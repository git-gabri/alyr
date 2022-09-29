#ifndef BLOCK_EXP_CALCULATOR_IPP_INCLUDED
#define BLOCK_EXP_CALCULATOR_IPP_INCLUDED

#include "alyr.hpp"

#include <cmath>

//Block renderer
template<map_fn_ptr_t map_fn, map_der_fn_ptr_t map_der_fn>
void alyr::internals::block_exp_calculator(const size_t& img_width, const size_t& img_height,
                                           const size_t& start_x, const size_t& start_y,
                                           const size_t& end_x, const size_t& end_y,
                                           std::vector<std::vector<long double>>& lyap_exp_matr){
    
    //Auxiliary variables
    //

    //Iterate over all the pixels in the block
    for(size_t x = start_x; x < end_x; ++x){
        for(size_t y = start_y; y < end_y; ++y){
            //Initialize r for iteration A and r for interation B
            const long double ra = std::lerp(fsettings.min_ra, fsettings.max_ra, static_cast<long double>(img_height - 1 - y) / static_cast<long double>(img_height  - 1));
            const long double rb = std::lerp(fsettings.min_rb, fsettings.max_rb, static_cast<long double>(x) / static_cast<long double>(img_width - 1));

            //Initialize xn, n-th element of the sequence to the initial value
            std::complex<long double> xn = fsettings.x0;

            //Initialize accumulator for Lyapunov exponent
            long double lyap_exp = 0;

            //Set iteration count to 0
            size_t iter_count = 0;

            //Main iterating loop
            while(iter_count < rsettings.max_iter && std::isfinite(lyap_exp)){
                //Calculate current r to use
                const rxtype current_rx_type = rx_sequence[iter_count % rx_sequence.size()];

                //Set selected r
                long double selected_rx = 0;
                switch(current_rx_type){
                    default:
                    case rxtype::A:
                        selected_rx = ra;
                        break;

                    case rxtype::B:
                        selected_rx = rb;
                        break;
                }

                //Update the value of xn and of the Lyapunov exponent
                xn        = (*map_fn)(xn, selected_rx);
                if(iter_count > rsettings.transient_iter)
                    lyap_exp += 0.5l * std::log(std::norm((*map_der_fn)(xn, selected_rx)));

                //Increment iteration count
                ++iter_count;
            }
            
            //Take average
            if(iter_count > rsettings.transient_iter)
                lyap_exp /= static_cast<long double>(iter_count - rsettings.transient_iter);
            else
                lyap_exp /= static_cast<long double>(iter_count);
            //std::cout << x << ", " << y << " : r = (a = " << ra << ", b = " << rb << ") : exp = " << lyap_exp << std::endl;

            //Compute color of pixel
            //image_to_write[x][y] = compute_color(lyap_exp, xn);
            //image_to_write[y][x] = (lyap_exp < 0 ? png::rgb_pixel(255, 255, 0) : png::rgb_pixel(0, 0, 255));
            lyap_exp_matr[y][x] = lyap_exp;
        }
    }
}

#endif