#ifndef ALYR_HPP_INCLUDED
#define ALYR_HPP_INCLUDED

#include "structs.hpp"
#include "aliases.hpp"

#include <vector>
#include <array>
#include <string>
#include <png++/png.hpp>

namespace alyr{
    //Initialize the number of threads to use in the render, can be changed later
    //Implementation:   alyr.cpp
    void init();

    //Parse command line options
    //Implementation:   parse_options.cpp
    int parse_options(std::vector<std::string> options);

    //Load palettes
    //Implementation:   load_palettes.cpp
    int load_palettes();

    //Render the image
    //Implementation:   rendering.cpp and others
    png::image<png::rgb_pixel> render();

    namespace internals{
        //-------------------------------------------------------
        //Private members
        extern fractalsettings_t    fsettings;
        extern imagesettings_t      isettings;
        extern colorsettings_t      csettings;
        extern rendersettings_t     rsettings;
        extern consolesettings_t    consettings;

        extern std::vector<rxtype> rx_sequence;

        extern std::vector<png::rgb_pixel> npalette;    //negative palette
        extern std::vector<png::rgb_pixel> ppalette;    //positive palette

        //-------------------------------------------------------
        //Private methods

        //Print various information about the current render
        //Implementation:   alyr.cpp
        void print_render_info();

        //Print errors and warnings
        //Implementation:   alyr.cpp
        void print_error(const std::string& msg);
        void print_warning(const std::string& msg);

        //Function to subdivide the image in "sectors" to parallelize jobs
        //Implementation: render.cpp
        std::vector<std::array<size_t, 4>> generate_sectors();

        //Function to return a function pointer to a block renderer depending on the settings
        //Implementation:   alyr.cpp
        block_exp_calc_fn_ptr_t get_block_exp_calc_ptr();

        //Lyapunov exponent calculator of all the pixels in a certain region
        //Implementation:   block_exp_calculator.ipp
        template<map_fn_ptr_t map_fn, map_der_fn_ptr_t map_der_fn>
        void block_exp_calculator(const size_t& img_width,  const size_t& img_height,
                                  const size_t& start_x,    const size_t& start_y,
                                  const size_t& end_x,      const size_t& end_y,
                                  std::vector<std::vector<long double>>& lyap_exp_matr);
        //Renderer of a certain region
        void block_renderer(const size_t& start_x,      const size_t& start_y,
                            const size_t& end_x,        const size_t& end_y,
                            const long double& max_pos, const long double& min_neg,
                            std::vector<std::vector<long double>>& lyap_exp_matr,
                            png::image<png::rgb_pixel>& img_to_color);

        //Save Lyapunov exponent matrix to file
        //Implementation:   save_load_lyap_exp_matr.cpp
        int save_lyap_exp_matrix(const std::vector<std::vector<long double>>& matr, const std::string& filename);

        //Load Lyapunov exponent matrix from file
        //Implementation:   save_load_lyap_exp_matr.cpp
        std::vector<std::vector<long double>> load_lyap_exp_matrix(const std::string& filename);

        //Compute color based on render data
        //Implementation:   block_renderer.cpp
        //png::rgb_pixel compute_color(const long double& lyap_exp, const std::complex<long double>& x);

        //Invert color of a pixel
        //Implementation:   render.cpp
        png::rgb_pixel invert_color(const png::rgb_pixel& c);

        //Draw crosshair in the middle of the image
        //Implementation:   render.cpp
        void draw_crosshair(png::image<png::rgb_pixel>& img);
    }
}

#include "./rendering/block_exp_calculator.ipp"

#endif