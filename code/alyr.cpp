#include "alyr.hpp"
#include "maps.hpp"

#include <thread>
#include <iostream>

fractalsettings_t       alyr::internals::fsettings{};
imagesettings_t         alyr::internals::isettings{};
colorsettings_t         alyr::internals::csettings{};
rendersettings_t        alyr::internals::rsettings{};
consolesettings_t       alyr::internals::consettings{};

std::vector<rxtype>     alyr::internals::rx_sequence{rxtype::A, rxtype::B};

std::vector<png::rgb_pixel> alyr::internals::npalette{};
std::vector<png::rgb_pixel> alyr::internals::ppalette{};

//Initialize the number of threads to use in the render, can be changed later
void alyr::init(){
    size_t max_t = std::thread::hardware_concurrency();
    if(max_t == 0)
        max_t = FALLBACK_NUM_THREADS;

    internals::rsettings.max_threads = max_t;
}

//Print various information about the current render
void alyr::internals::print_render_info(){
    using std::cout;
    using std::endl;

    cout << "Finish me" << endl;
}

//Print errors and warnings
void alyr::internals::print_error(const std::string& msg){
    std::cout << "[ERROR] : " << msg << "\n";
}
void alyr::internals::print_warning(const std::string& msg){
    std::cout << "[WARN] : " << msg << "\n";
}

//Function to return a function pointer to a block renderer depending on the settings
block_exp_calc_fn_ptr_t alyr::internals::get_block_exp_calc_ptr(){
    return &block_exp_calculator<
        &logmap<std::complex<long double>>,
        &logmap_der<std::complex<long double>>
    >;
}