#include "alyr.hpp"
#include "threadpool.hpp"

#include <array>
#include <algorithm>
#define vcout if(consettings.verbose_output) cout

using namespace std;
using namespace alyr::internals;

//--------------------------------------------------------------------------------------------------
png::image<png::rgb_pixel> alyr::render(){
    //----------------------
    // STEP 0:
    // - divide the image in blocks to parallelize the rendering job,
    // - create threadpool

    //To better distribute the workload between all the threads, the image gets divided into sectors, then
    //when one thread working on a sector finishes, we launch another one to work on another sector, so that we
    //(almost) always have all the threads working on a sector
    vector<array<size_t, 4>> sectors = {};

    //Two-dimensional vector containing the Ly. exp for each calculated point
    vcout << "Allocating lambda matrix in RAM... " << flush;
    vector<vector<long double>> lyap_exponents(isettings.image_height, vector<long double>(isettings.image_width, 0));
    vcout << "Done!" << endl;

    //Creating sectors onto which thread can work in parallel
    for(size_t i = 0; i < isettings.image_height; i += rsettings.max_sector_size) {
        for(size_t j = 0; j < isettings.image_width; j += rsettings.max_sector_size) {
            size_t start_x = j;
            size_t start_y = i;
            size_t end_x   = j + min((size_t)rsettings.max_sector_size, isettings.image_width - j);
            size_t end_y   = i + min((size_t)rsettings.max_sector_size, isettings.image_height - i);

            sectors.push_back({start_x, start_y, end_x, end_y});
        }
    }
    size_t total_sectors = sectors.size();

    //Function pointer to Lyapunov exp calculator
    block_exp_calc_fn_ptr_t block_exp_calc_pointer = get_block_exp_calc_ptr();

    //Print info if required
    if(consettings.verbose_output) print_render_info();

    //Create threadpool for parallel jobs
    threadpool renderpool(rsettings.max_threads);

    //----------------------
    // STEP 1:
    // - calculate the Lyapunov exponent for each pixel
    
    //vector of void to ensure the main threads proceeds only after all the sectors have rendered
    vector<future<void>> completed_sectors;

    //For every sector
    for(auto s : sectors){
        size_t start_x = s[0];
        size_t start_y = s[1];
        size_t end_x   = s[2];
        size_t end_y   = s[3];

        //Enqueue a job to the renderpool
        completed_sectors.emplace_back(
            renderpool.enqueue(
                block_exp_calc_pointer,     //Block exponent calculator
                isettings.image_width,      //Width of the image
                isettings.image_height,     //Height of the image
                start_x, start_y,           //(x,y) starting position
                end_x, end_y,               //(x,y) ending position
                ref(lyap_exponents)         //Reference to matrix of exponents
            )
        );
    }

    vcout << "Completed sectors (exp): 0/" << total_sectors << "\r" << flush;
    //Once all the jobs are enqueued, wait for all of them to finish
    for(size_t i = 0; i < completed_sectors.size(); ++i){
        completed_sectors[i].get();
        vcout << "Completed sectors (exp): " << i << "/" << total_sectors << "\r" << flush;
    }
    vcout << "Completed sectors (exp): " << total_sectors << "/" << total_sectors << endl;
    completed_sectors.clear();

    //----------------------
    // STEP 2:
    // - perform statystical analysis over all the Lyapunov exponents
    //   - find maximum and minimum
    //   - ...

    vcout << "Statistical analysis of the exponents:" << endl;
    //Sort the exponents in positive and negative ones
    size_t pos_inf_count = 0;
    size_t neg_inf_count = 0;
    size_t nan_count = 0;
    vector<long double> abs_pos_exponents{};
    vector<long double> abs_neg_exponents{};
    for(size_t y = 0; y < isettings.image_height; ++y){
        for(size_t x = 0; x < isettings.image_width; ++x){
            if(isfinite(lyap_exponents[y][x])){
                if(lyap_exponents[y][x] >= 0)
                    abs_pos_exponents.push_back(lyap_exponents[y][x]);
                else
                    abs_neg_exponents.push_back(-lyap_exponents[y][x]);
            }
            else if(isnan(lyap_exponents[y][x]))
                ++nan_count;
            else{
                if(lyap_exponents[y][x] > 0)
                    ++pos_inf_count;
                else
                    ++neg_inf_count;
            }
        }
    } 
    vcout << "  - Positive count: " << abs_pos_exponents.size() << endl;
    vcout << "  - Positive inf. : " << pos_inf_count << endl;
    vcout << "  - Negative count: " << abs_neg_exponents.size() << endl;
    vcout << "  - Negative inf. : " << neg_inf_count << endl;
    vcout << "  - NaN count     : " << nan_count << endl;
    sort(abs_pos_exponents.begin(), abs_pos_exponents.end());
    sort(abs_neg_exponents.begin(), abs_neg_exponents.end());
    vcout << "  - Pos. exponents : [" <<  abs_pos_exponents.front() << ", " << abs_pos_exponents.back()  << "] clamped in [" << rsettings.lower_pos_clamp << ", " << rsettings.upper_pos_clamp << "]" << endl;
    vcout << "  - Neg. exponents : [" << -abs_neg_exponents.back() << ", " << -abs_neg_exponents.front() << "] clamped in [" << rsettings.lower_neg_clamp << ", " << rsettings.upper_neg_clamp << "]" << endl;
    const long double max_pos = abs_pos_exponents.back();
    const long double min_neg = -abs_neg_exponents.back();

    //----------------------
    // STEP 3:
    // - color the image block by block

    //Image of the fractal
    vcout << "Allocating image in RAM... " << flush;
    png::image<png::rgb_pixel> fractal_image(isettings.image_width, isettings.image_height);
    vcout << "Done!" << endl;

    //Function pointer to the block renderer
    block_renderer_fn_ptr_t block_renderer_pointer = &block_renderer;

    //For every sector
    for(auto s : sectors){
        size_t start_x = s[0];
        size_t start_y = s[1];
        size_t end_x   = s[2];
        size_t end_y   = s[3];

        //Enqueue a job to the renderpool
        completed_sectors.emplace_back(
            renderpool.enqueue(
                block_renderer_pointer,     //Block renderer
                start_x, start_y,           //(x,y) starting position
                end_x, end_y,               //(x,y) ending position
                max_pos,                    //Maximum positive exponent
                min_neg,                    //Minimum negative exponent
                ref(lyap_exponents),        //Reference to matrix of exponents
                ref(fractal_image)          //Reference to image to update pixels
            )
        );
    }

    vcout << "Completed sectors (color): 0/" << total_sectors << "\r" << flush;
    //Once all the jobs are enqueued, wait for all of them to finish
    for(size_t i = 0; i < completed_sectors.size(); ++i){
        completed_sectors[i].get();
        vcout << "Completed sectors (color): " << i << "/" << total_sectors << "\r" << flush;
    }
    vcout << "Completed sectors (color): " << total_sectors << "/" << total_sectors << endl;
    completed_sectors.clear();

    //----------------------
    // STEP 4:
    // - draw crosshair if required

    if(csettings.draw_crosshair)
        draw_crosshair(fractal_image);

    return fractal_image;
}

//--------------------------------------------------------------------------------------------------
//Invert color of a pixel
png::rgb_pixel alyr::internals::invert_color(const png::rgb_pixel& c){
    return png::rgb_pixel(255 - c.red, 255 - c.green, 255 - c.blue);
}

//Draw crosshair in the middle of the image
void alyr::internals::draw_crosshair(png::image<png::rgb_pixel>& img){
    const size_t halfWidth = isettings.image_width / 2;
    const bool evenWidth = (isettings.image_width % 2 == 0);
    const size_t halfHeight = isettings.image_height / 2;
    const bool evenHeight = (isettings.image_height % 2 == 0);

    //Draw vertical center line
    for(size_t i = 0; i < isettings.image_height; ++i){
        img[i][halfWidth] = invert_color(img[i][halfWidth]);
        //If the image has an even number of pixels in width, make the line 2 pixels thick
        if(evenWidth)
            img[i][halfWidth - 1] = invert_color(img[i][halfWidth - 1]);
    }

    //Draw horizontal center line
    for(size_t i = 0; i < isettings.image_width; ++i){
        img[halfHeight][i] = invert_color(img[halfHeight][i]);
        //If the image has an even number of pixels in height, make the line 2 pixels thick
        if(evenHeight)
            img[halfHeight - 1][i] = invert_color(img[halfHeight - 1][i]);
    }
}