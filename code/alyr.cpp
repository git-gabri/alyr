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

    //Map being rendered
    cout << "Rendering      : ";
    std::string map_type_str;
    switch(fsettings.map_type){
        case mtype::logmap: map_type_str = "logistic map"; break;
        case mtype::circmap: map_type_str = "circle map"; break;
        case mtype::gaussmap: map_type_str = "Gauss map"; break;
        case mtype::custom: map_type_str = "custom map"; break;
        default:
        case mtype::unknown: map_type_str = "unknown/undocumented"; break;
    }
    cout << map_type_str << endl;

    //Sequence used
    cout << "Sequence       : ";
    for(auto it = rx_sequence.begin(); it != rx_sequence.end(); ++it){
        switch(*it){
            case rxtype::A: cout << 'A'; break;
            case rxtype::B: cout << 'B'; break;
            case rxtype::C: cout << 'C'; break;
        }
    }
    cout << endl;

    //Image size
    cout << "Image size     : " << isettings.image_width << "x" << isettings.image_height << endl;

    //Sectors
    cout << "Sectors up to  : " << rsettings.max_sector_size << "x" << rsettings.max_sector_size << endl;

    //Iterations
    cout << "Iterations     : " << rsettings.max_iter << endl;
    cout << "Transient iter.: " << rsettings.transient_iter << endl;

    //Limits of ra, rb and rc
    cout << "Limits of ra   : [" << fsettings.min_ra << ", " << fsettings.max_ra << "], span : " << fsettings.max_ra - fsettings.min_ra << endl;
    cout << "Limits of rb   : [" << fsettings.min_rb << ", " << fsettings.max_rb << "], span : " << fsettings.max_rb - fsettings.min_rb << endl;
    cout << "Limits of rc   : [" << fsettings.min_rc << ", " << fsettings.max_rc << "], span : " << fsettings.max_rc - fsettings.min_rc << endl;
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