#ifndef PARSE_OPTIONS_HPP_INCLUDED
#define PARSE_OPTIONS_HPP_INCLUDED

#include "alyr.hpp"

#include <iostream>
#include <map>

//Utility functions
//Return values:
// 0 -> OK
//!0 -> some error occurred
int string_to_st(const std::string& str, size_t& ull);
int string_to_st(const std::vector<std::string>& vec, const std::vector<std::string>::iterator& it, size_t& ull);
int string_to_ld(const std::string& str, long double& ld);
int string_to_ld(const std::vector<std::string>& vec, const std::vector<std::string>::iterator& it, long double& ld);
int string_to_int(const std::string& str, int& i);
int string_to_int(const std::vector<std::string>& vec, const std::vector<std::string>::iterator& it, int& i);

int extract_n_numbers_from_vec(const std::vector<std::string>& stringvec, const size_t& n, std::vector<long double>& extracted_numbers);

//-------------------------------------------------------------------------------
//Command line option definition and maps used as lookup tables
enum class cmdline_option{
    print_help,
    enable_verbose,

    set_width, set_height,
    set_output_image_filename,

    set_sector_size,
    set_max_threads,

    set_map,
    set_x0_re, set_x0_im,
    set_max_iterations,

    set_npalette_filename,
    set_ppalette_filename,
    set_coloring_mode,
    enable_crosshair,
    unknown
};

const std::map<cmdline_option, size_t> map_cmdlineopt_num_elem_to_pop{
    {cmdline_option::print_help, 1},
    {cmdline_option::enable_verbose, 1},

    {cmdline_option::set_width, 2},
    {cmdline_option::set_height, 2},
    {cmdline_option::set_output_image_filename, 2},

    {cmdline_option::set_sector_size, 2},
    {cmdline_option::set_max_threads, 2},

    {cmdline_option::set_map, 2},
    {cmdline_option::set_x0_re, 2},
    {cmdline_option::set_x0_im, 2},
    {cmdline_option::set_max_iterations, 2},

    {cmdline_option::set_npalette_filename, 2},
    {cmdline_option::set_ppalette_filename, 2},
    {cmdline_option::set_coloring_mode, 2},
    {cmdline_option::enable_crosshair, 1},
    {cmdline_option::unknown, 1}
};

const std::map<std::string, cmdline_option> map_str_to_cmdlineopt{
    {"-H",              cmdline_option::print_help},
    {"--help",          cmdline_option::print_help},
    {"-v",              cmdline_option::enable_verbose},
    {"--verbose",       cmdline_option::enable_verbose},

    {"-w",              cmdline_option::set_width},
    {"--width",         cmdline_option::set_width},
    {"-h",              cmdline_option::set_height},
    {"--height",        cmdline_option::set_height},
    {"-o",              cmdline_option::set_output_image_filename},
    {"--output-image-filename", cmdline_option::set_output_image_filename},

    {"-S",              cmdline_option::set_sector_size},
    {"--sector-size",   cmdline_option::set_sector_size},
    {"-T",              cmdline_option::set_max_threads},
    {"--max-threads",   cmdline_option::set_max_threads},

    {"-m",              cmdline_option::set_map},
    {"--map",           cmdline_option::set_map},
    {"-xr",             cmdline_option::set_x0_re},
    {"--x0-re",         cmdline_option::set_x0_re},
    {"--real",          cmdline_option::set_x0_re},
    {"-xi",             cmdline_option::set_x0_im},
    {"--x0-im",         cmdline_option::set_x0_im},
    {"--imag",          cmdline_option::set_x0_im},
    {"-t",              cmdline_option::set_max_iterations},
    {"--max-iter",      cmdline_option::set_max_iterations},

    {"-np",                 cmdline_option::set_npalette_filename},
    {"--npalette-filename", cmdline_option::set_npalette_filename},
    {"-pp",                 cmdline_option::set_ppalette_filename},
    {"--ppalette-filename", cmdline_option::set_ppalette_filename},
    {"-c",              cmdline_option::set_coloring_mode},
    {"--coloring-mode", cmdline_option::set_coloring_mode},
    {"-C",              cmdline_option::enable_crosshair},
    {"--crosshair",     cmdline_option::enable_crosshair}
};

const std::map<std::string, mtype> map_string_to_mtype{
    {"logmap",      mtype::logmap},
    {"circmap",     mtype::circmap},
    {"gaussmap",    mtype::gaussmap},
    {"custom",      mtype::custom}
};

const std::map<std::string, coloring_mode> map_string_to_coloring_mode{
    {"binary",      coloring_mode::binary},
    {"linear",      coloring_mode::linear}
};

#endif