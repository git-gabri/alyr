#ifndef STRUCTS_HPP_INCLUDED
#define STRUCTS_HPP_INCLUDED

#include <string>
#include <vector>
#include <complex>

//Map type enum
enum class mtype{
    logmap, circmap, gaussmap,
    custom,
    unknown
};

//Possible values for the constants "rx"s used in the map formular
enum class rxtype{
    A, B, C
};

////Renderer type enum
//enum class rtype{
//    basic,
//    unknown
//};

//Coloring mode enum
enum class coloring_mode{
    binary, linear,
    unknown
};

//Struct containing all the settings for the fractal
struct fractalsettings_t {
    mtype map_type;

    std::complex<long double> x0;
    long double min_ra;
    long double max_ra;

    long double min_rb;
    long double max_rb;

    long double min_rc;
    long double max_rc;

    fractalsettings_t(
        mtype _map_type = mtype::logmap,
        long double x0_re = 0.5l,
        long double x0_im = 0,
        long double _min_ra = 0,
        long double _max_ra = 4,
        long double _min_rb = 0,
        long double _max_rb = 4,
        long double _min_rc = 0,
        long double _max_rc = 0
    ) :
    map_type(_map_type),
    x0(x0_re, x0_im),
    min_ra(_min_ra),
    max_ra(_max_ra),
    min_rb(_min_rb),
    max_rb(_max_rb),
    min_rc(_min_rc),
    max_rc(_max_rc) {}
};

//Struct containing all the settings for the output image
struct imagesettings_t {
    size_t image_width;
    size_t image_height;
    std::string image_name;

    imagesettings_t(
        size_t _imageWidth = 1000,
        size_t _imageHeight = 1000,
        std::string _imageName = {"fractal"}
    ) :
    image_width(_imageWidth), image_height(_imageHeight),
    image_name(_imageName) {}
};

//Struct containing all the settings for the coloring of the image
struct colorsettings_t {
    coloring_mode cmode;
    std::string name_neg_palette;
    std::string name_pos_palette;

    bool draw_crosshair;

    colorsettings_t(
        coloring_mode _cmode = coloring_mode::linear,
        std::string _name_neg_palette = {"npalette"},
        std::string _name_pos_palette = {"ppalette"},
        bool _draw_crosshair = false
    ) :
    cmode(_cmode),
    name_neg_palette(_name_neg_palette),
    name_pos_palette(_name_pos_palette),
    draw_crosshair(_draw_crosshair) {}
};

//Struct containing all the settings for the rendering of the fractal
struct rendersettings_t {
    //rtype renderer_type;

    size_t max_iter;
    size_t transient_iter;
    size_t max_sector_size;
    size_t max_threads;

    bool save_exp_matrix;
    bool load_exp_matrix;
    bool skip_coloring;

    long double lower_pos_clamp;
    long double upper_pos_clamp;
    long double lower_neg_clamp;
    long double upper_neg_clamp;

    std::string lyap_exp_matr_out_filename;
    std::string lyap_exp_matr_in_filename;
    
    rendersettings_t(
        //const rtype& _renderer_type = rtype::basic,
        const size_t& _max_iter = 2000,
        const size_t& _transient_iter = 200,
        const size_t& _max_sector_size = 64,
        const size_t& _max_threads = 1,
        const bool& _save_matr = false,
        const bool& _load_matr = false,
        const bool& _skip_coloring = false,
        const long double& _low_pos_clamp = 0,
        const long double& _up_pos_clamp = 10000,
        const long double& _low_neg_clamp = -10000,
        const long double& _up_neg_clamp = 0,
        const std::string& _out_matr_filename = "exponent_matrix",
        const std::string& _in_matr_filename = "exponent_matrix"
    ) :
    //renderer_type(_renderer_type),
    max_iter(_max_iter),
    transient_iter(_transient_iter),
    max_sector_size(_max_sector_size),
    max_threads(_max_threads),
    save_exp_matrix(_save_matr),
    load_exp_matrix(_load_matr),
    skip_coloring(_skip_coloring),
    lower_pos_clamp(_low_pos_clamp),
    upper_pos_clamp(_up_pos_clamp),
    lower_neg_clamp(_low_neg_clamp),
    upper_neg_clamp(_up_neg_clamp),
    lyap_exp_matr_out_filename(_out_matr_filename),
    lyap_exp_matr_in_filename(_in_matr_filename)
    {}
};

//Struct containing information of a single rendered pixel
//struct pixel_t{
//    unsigned char red, green, blue, alpha;
//
//    //T -> to compute
//    //C -> directly computed
//    //A -> approximated (from rectangle guessing, ecc...)
//    unsigned char status;
//
//    std::vector<std::complex<long double>> history;
//    
//    long double pickover_min_distance;
//};

//Struct containing all the settings for printing text on the console
struct consolesettings_t {
    int verbose_output;
    int colored_output;
    int suppress_warnings;
    int suppress_errors;

    consolesettings_t(
        const int& _verbose_output = 0,
        const int& _colored_output = 0,
        const int& _suppress_warnings = 0,
        const int& _suppress_errors = 0
    ) :
    verbose_output(_verbose_output),
    colored_output(_colored_output),
    suppress_warnings(_suppress_warnings),
    suppress_errors(_suppress_errors) {}
};

#endif