#include "alyr.hpp"
#include "threadpool.hpp"

#include <array>
#include <algorithm>
#define vcout if(consettings.verbose_output) cout

using namespace std;
using namespace alyr::internals;

//Function to subdivide the image in "sectors" to parallelize jobs
//Implementation: render.cpp
vector<array<size_t, 4>> alyr::internals::generate_sectors(){
    //To better distribute the workload between all the threads, the image gets divided into sectors, then
    //when one thread working on a sector finishes, we launch another one to work on another sector, so that we
    //(almost) always have all the threads working on a sector
    vector<array<size_t, 4>> sectors = {};

    //Creating sectors onto which thread can work in parallel
    for(size_t i = 0; i < isettings.image_height; i += rsettings.max_sector_size) {
        for(size_t j = 0; j < isettings.image_width; j += rsettings.max_sector_size) {
            size_t start_x = j;
            size_t start_y = i;
            size_t end_x   = min((size_t)rsettings.max_sector_size + j, isettings.image_width);
            size_t end_y   = min((size_t)rsettings.max_sector_size + i, isettings.image_height);

            sectors.push_back({start_x, start_y, end_x, end_y});
        }
    }

    return sectors;
}

//--------------------------------------------------------------------------------------------------
png::image<png::rgb_pixel> alyr::render(){
    // The render is divided into 3 steps
    // 1) generate the exponents matrix, either by performing calculations or loading it from a file
    // 2) save the matrix to a file
    // 3) color the exponents matrix, if it's not required to skip coloring

    //----------------------
    // STEP 1: generate the exponents matrix, either by performing calculations or loading it from a file
    // 
    // - allocate the matrix
    // - allocate a vector of block (sectors) into which the image is divided to parallelize the rendering job
    // - create a threadpool and a vector of future<void> for the multithreading part
    // calculations
    // - generate sectors to parallelize the rendering job
    // - get pointer to exponent calculator function
    // - print info
    // - enqueue jobs in threadpool
    // - print completion state
    // OR
    // load from file
    // - load matrix from file
    // - generate sectors

    //Two-dimensional vector containing the Ly. exp for each calculated point
    vector<vector<long double>> lyap_exponents;
    //Divide the image into block (sectors)
    vector<array<size_t, 4>> sectors;

    //Create threadpool for parallel jobs
    threadpool renderpool(rsettings.max_threads);
    //Vector of future to wait for all the other threads to continue the rendering after all the jobs on all the sectors is finished
    vector<future<void>> completed_sectors;

    //If calculations are necessary...
    if(!rsettings.load_exp_matrix){
        //Pre-allocate the matrix
        vcout << "Allocating lambda matrix in RAM... " << flush;
        lyap_exponents = vector<vector<long double>>(isettings.image_height, vector<long double>(isettings.image_width, 0));
        vcout << "Done!" << endl;

        //Generate the sectors
        sectors = generate_sectors();

        //Function pointer to Lyapunov exp calculator
        block_exp_calc_fn_ptr_t block_exp_calc_pointer = get_block_exp_calc_ptr();

        //Print info if required
        if(rsettings.load_exp_matrix == false &&  consettings.verbose_output == true)
            print_render_info();

        //Enqueue jobs
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


        //Print completion state
        const size_t total_sectors = sectors.size();
        vcout << "Completed sectors (exp): 0/" << total_sectors << "\r" << flush;
        //Once all the jobs are enqueued, wait for all of them to finish
        for(size_t i = 0; i < completed_sectors.size(); ++i){
            completed_sectors[i].get();
            vcout << "Completed sectors (exp): " << i << "/" << total_sectors << "\r" << flush;
        }
        vcout << "Completed sectors (exp): " << total_sectors << "/" << total_sectors << endl;
        completed_sectors.clear();
    }
    //If matrix is loaded from file...
    else{
        //Load data from file
        vcout << "Loading lambda matrix in RAM... " << flush;
        lyap_exponents = load_lyap_exp_matrix(rsettings.lyap_exp_matr_in_filename);
        vcout << "Done!" << endl;

        //Check for validity of data
        if(lyap_exponents == vector<vector<long double>>{vector<long double>{}}){
            print_error("invalid exponent matrix loaded from file");
            //Return 1x1 empty image
            return png::image<png::rgb_pixel>(1, 1);
        }

        //Update the image settings accordingly
        isettings.image_height = lyap_exponents.size();             //Number of rows
        isettings.image_width  = lyap_exponents.front().size();     //Number of columns

        //Generate the sectors with the new settings
        sectors = generate_sectors();
    }

    //----------------------
    // STEP 2: save the matrix to a file (if required)
    // 
    // - if required to save, continue, else go to step 3
    // - save the matrix to a file
    // - reload it to RAM
    // - check if what has been saved is identical to the initial matrix

    if(rsettings.save_exp_matrix){
        vcout << "Saving... " << flush;
        if(save_lyap_exp_matrix(lyap_exponents, rsettings.lyap_exp_matr_out_filename) == 0){
            vcout << "done. Checking... " << flush;

            if(load_lyap_exp_matrix(rsettings.lyap_exp_matr_out_filename) == lyap_exponents){
                vcout << "OK" << endl;
            }
            else{
                vcout << "ERROR" << endl;
                print_warning("exponent matrix hasn't been saved correctly to file");
            }
        }
        else{
            vcout << "ERROR" << endl;
            print_error("exponent matrix couldn't be saved");
            //Return 1x1 empty image
            return png::image<png::rgb_pixel>(1, 1);
        }
    }

    //----------------------
    // STEP 3: color the exponents matrix
    //
    // - statistical analysis of the exponents (find maximum, minimum)
    // - print results
    // - allocate image in RAM
    // - get pointer to renderer function
    // - enqueue coloring jobs
    // - print completion state
    // - if required to draw crosshair, draw crosshair

    //Statistical analysis
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

    //If coloring should be skipped
    if(rsettings.skip_coloring){
        //Return 1x1 empty image
        return png::image<png::rgb_pixel>(1, 1);
    }
    //Else color the image
    else{
        //Allocate image of the fractal
        vcout << "Allocating image in RAM... " << flush;
        png::image<png::rgb_pixel> fractal_image(isettings.image_width, isettings.image_height);
        vcout << "Done!" << endl;

        //Function pointer to the block renderer
        block_renderer_fn_ptr_t block_renderer_pointer = &block_renderer;

        //Enqueue jobs
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

        const size_t total_sectors = sectors.size();
        vcout << "Completed sectors (color): 0/" << total_sectors << "\r" << flush;
        //Once all the jobs are enqueued, wait for all of them to finish
        for(size_t i = 0; i < completed_sectors.size(); ++i){
            completed_sectors[i].get();
            vcout << "Completed sectors (color): " << i << "/" << total_sectors << "\r" << flush;
        }
        vcout << "Completed sectors (color): " << total_sectors << "/" << total_sectors << endl;
        completed_sectors.clear();

        //Draw crosshair if required
        if(csettings.draw_crosshair)
            draw_crosshair(fractal_image);

        return fractal_image;
    }
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