#include "parse_options.hpp"
#include "help.hpp"

#include <regex>

using namespace std;
using namespace alyr::internals;

int string_to_st(const string& str, size_t& ull){
    size_t tmp;
    try{
        tmp = stoull(str);
    }
    catch(...){
        print_error("couldn't convert \"" + str + "\" to size_t");
        return 1;
    }

    ull = tmp;
    return 0;
}

int string_to_st(const std::vector<std::string>& vec, const std::vector<std::string>::iterator& it, size_t& ull)
    {return (it == vec.end() ? 2 : string_to_st((*it), ull));}

int string_to_ld(const std::string& str, long double& ld){
    long double tmp;
    try{
        tmp = stold(str);
    }
    catch(...){
        print_error("couldn't convert \"" + str + "\" to long double");
        return 1;
    }

    ld = tmp;
    return 0;
}

int string_to_ld(const std::vector<std::string>& vec, const std::vector<std::string>::iterator& it, long double& ld)
    {return (it == vec.end() ? 2 : string_to_ld(*it, ld));}

int string_to_int(const string& str, int& i){
    int tmp;
    try{
        tmp = stoi(str);
    }
    catch(...){
        print_error("couldn't convert \"" + str + "\" to int");
        return 1;
    }

    i = tmp;
    return 0;
}

int string_to_int(const std::vector<std::string>& vec, const std::vector<std::string>::iterator& it, int& i)
    {return (it == vec.end() ? 2 : string_to_int(*it, i));}

int extract_n_numbers_from_vec(const vector<string>& stringvec, const size_t& n, vector<long double>& extracted_numbers){
    if(stringvec.size() < n){
        print_error("can't extract " + to_string(n) + " arguments from string list");
        return 1;
    }

    vector<long double> tmp;
    for(size_t i = 0; i < n; ++i){
        tmp.emplace_back();
        if(string_to_ld(stringvec[i], tmp.back()))
            return 2;
    }

    extracted_numbers = tmp;
    return 0;
}

//Function to parse a list of options, passed to the programs either from argv of by reading a configuration file
//
//Return values:
//-1 -> ERR :   unrecognized option
// 0 -> OK  :   all the parsing happened succesfully
// 1 -> OK  :   help was printed
// 2 -> ERR :   generic error
int alyr::parse_options(std::vector<std::string> options){
    //---------------------------------------------------------------------------------
    //START PARSING
    while(!options.empty()){
        //Load the first element in "options", which is the first string to be parsed
        const auto front_element = options.front();
        cmdline_option current_option = cmdline_option::unknown;

        //If the string is recognized as a valid flag to set a certain option, convert it to the relatice cmdline_option
        if(map_str_to_cmdlineopt.contains(front_element))
            current_option = map_str_to_cmdlineopt.at(front_element);

        //Variable used to know how many elements to pop from "options" after the parsing of the first element is finished
        size_t elements_to_pop = map_cmdlineopt_num_elem_to_pop.at(current_option);

        //Main switch case to differentiate the behaviour for the different options
        switch(current_option){
            //---------------------------------------------------------------------
            case cmdline_option::print_help:
                print_help(cout);
                return 1;
                break;
            
            //---------------------------------------------------------------------
            case cmdline_option::enable_verbose:
                consettings.verbose_output = true;
                break;

            //---------------------------------------------------------------------
            case cmdline_option::set_width:
            {   size_t tmp_width;
                if(string_to_st(options, options.begin() + 1, tmp_width)){
                    print_error("unspecified/specified image width is invalid");
                    return 2;
                }
                else
                    isettings.image_width = tmp_width;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_height:
            {   size_t tmp_height;
                if(string_to_st(options, options.begin() + 1, tmp_height)){
                    print_error("unspecified/specified image height is invalid");
                    return 2;
                }
                else
                    isettings.image_height = tmp_height;
            }  break;

            //---------------------------------------------------------------------
            case cmdline_option::set_output_image_filename:
                if(options.size() < 2){
                    print_error("unspecified/specified output image filename is invalid");
                    return 2;
                }
                else
                    isettings.image_name = *(options.begin() + 1);
                break;

            //---------------------------------------------------------------------
            case cmdline_option::save_lyap_exp_matrix:
                if(options.size() < 2){
                    print_error("unspecified/specified output matrix filename is invalid");
                    return 2;
                }
                else{
                    rsettings.save_exp_matrix = true;
                    rsettings.lyap_exp_matr_out_filename = *(options.begin() + 1);
                }
                break;

            //---------------------------------------------------------------------
            case cmdline_option::load_lyap_exp_matrix:
                if(options.size() < 2){
                    print_error("unspecified/specified input matrix filename is invalid");
                    return 2;
                }
                else{
                    rsettings.load_exp_matrix = true;
                    rsettings.lyap_exp_matr_in_filename = *(options.begin() + 1);
                }
                break;

            //---------------------------------------------------------------------
            case cmdline_option::skip_coloring:
                rsettings.skip_coloring = true;
                break;

            //---------------------------------------------------------------------
            case cmdline_option::set_sector_size:
            {   size_t tmp_secsize;
                if(string_to_st(options, options.begin() + 1, tmp_secsize)){
                    print_error("unspecified/specified sector size is invalid");
                    return 2;
                }
                else
                    rsettings.max_sector_size = tmp_secsize;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_max_threads:
            {   size_t tmp_max_threads;
                if(string_to_st(options, options.begin() + 1, tmp_max_threads)){
                    print_error("unspecified/specified maximum number of threads is invalid");
                    return 2;
                }
                else
                    rsettings.max_threads = tmp_max_threads;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_map:
            {   mtype tmp_map_type = mtype::unknown;
                if(options.size() < 2){
                    print_error("not enought arguments have been provided to set the map");
                    return 2;
                }

                const string tmp_map_str = *(options.begin() + 1);
                if(map_string_to_mtype.contains(tmp_map_str))
                    tmp_map_type = map_string_to_mtype.at(tmp_map_str);
                else{
                    print_error("unspecified/specified fractal is invalid");
                    return 2;
                }

                fsettings.map_type = tmp_map_type;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_sequence:
            {   if(options.size() < 2){
                    print_error("not enought arguments have been provided to set the sequence");
                    return 2;
                }

                const string tmp_seq = *(options.begin() + 1);
                const regex re_seq{R"foo(^[ABC]+$)foo"};
                if(regex_match(tmp_seq, re_seq)){
                    rx_sequence.clear();

                    for(auto seq_it = tmp_seq.begin(); seq_it != tmp_seq.end(); ++seq_it){
                        switch(*seq_it){
                            default:
                            case 'A':   rx_sequence.push_back(rxtype::A);   break;
                            case 'B':   rx_sequence.push_back(rxtype::B);   break;
                            case 'C':   rx_sequence.push_back(rxtype::C);   break;
                        }
                    }
                }
                else{
                    print_error("unspecified/specified sequence is invalid");
                    return 2;
                }

            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_x0_re:
            {   long double tmp_real;
                if(string_to_ld(options, options.begin() + 1, tmp_real)){
                    print_error("unspecified/specified real part for offset from the center is invalid");
                    return 2;
                }
                else
                    fsettings.x0.real(tmp_real);
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_x0_im:
            {   long double tmp_imag;
                if(string_to_ld(options, options.begin() + 1, tmp_imag)){
                    print_error("unspecified/specified imaginary part for offset from the center is invalid");
                    return 2;
                }
                else
                    fsettings.x0.imag(tmp_imag);
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_min_ra:
            {   long double tmp_min;
                if(string_to_ld(options, options.begin() + 1, tmp_min)){
                    print_error("unspecified/specified value for the minimum ra is invalid");
                    return 2;
                }
                else
                    fsettings.min_ra = tmp_min;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_max_ra:
            {   long double tmp_max;
                if(string_to_ld(options, options.begin() + 1, tmp_max)){
                    print_error("unspecified/specified value for the maximum ra is invalid");
                    return 2;
                }
                else
                    fsettings.max_ra = tmp_max;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_min_rb:
            {   long double tmp_min;
                if(string_to_ld(options, options.begin() + 1, tmp_min)){
                    print_error("unspecified/specified value for the minimum rb is invalid");
                    return 2;
                }
                else
                    fsettings.min_rb = tmp_min;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_max_rb:
            {   long double tmp_max;
                if(string_to_ld(options, options.begin() + 1, tmp_max)){
                    print_error("unspecified/specified value for the maximum rb is invalid");
                    return 2;
                }
                else
                    fsettings.max_rb = tmp_max;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_min_rc:
            {   long double tmp_min;
                if(string_to_ld(options, options.begin() + 1, tmp_min)){
                    print_error("unspecified/specified value for the minimum rc is invalid");
                    return 2;
                }
                else
                    fsettings.min_rc = tmp_min;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_max_rc:
            {   long double tmp_max;
                if(string_to_ld(options, options.begin() + 1, tmp_max)){
                    print_error("unspecified/specified value for the maximum rc is invalid");
                    return 2;
                }
                else
                    fsettings.max_rc = tmp_max;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_max_iterations:
            {   size_t tmp_max_iter;
                if(string_to_st(options, options.begin() + 1, tmp_max_iter)){
                    print_error("unspecified/specified maximum number of iterations is invalid");
                    return 2;
                }
                else
                    rsettings.max_iter = tmp_max_iter;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_transient_iterations:
            {   size_t tmp_transient_iter;
                if(string_to_st(options, options.begin() + 1, tmp_transient_iter)){
                    print_error("unspecified/specified number of transient iterations is invalid");
                    return 2;
                }
                else
                    rsettings.transient_iter = tmp_transient_iter;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_low_pos_clamp:
            {   long double tmp_low;
                if(string_to_ld(options, options.begin() + 1, tmp_low)){
                    print_error("unspecified/specified lower clamping value for positive exponents is invalid");
                    return 2;
                }
                else
                    rsettings.lower_pos_clamp = tmp_low;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_upp_pos_clamp:
            {   long double tmp_upp;
                if(string_to_ld(options, options.begin() + 1, tmp_upp)){
                    print_error("unspecified/specified upper clamping value for positive exponents is invalid");
                    return 2;
                }
                else
                    rsettings.upper_pos_clamp = tmp_upp;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_low_neg_clamp:
            {   long double tmp_low;
                if(string_to_ld(options, options.begin() + 1, tmp_low)){
                    print_error("unspecified/specified lower clamping value for negative exponents is invalid");
                    return 2;
                }
                else
                    rsettings.lower_neg_clamp = tmp_low;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_upp_neg_clamp:
            {   long double tmp_upp;
                if(string_to_ld(options, options.begin() + 1, tmp_upp)){
                    print_error("unspecified/specified upper clamping value for negative exponents is invalid");
                    return 2;
                }
                else
                    rsettings.upper_neg_clamp = tmp_upp;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::set_npalette_filename:
                if(options.size() < 2)
                    return 2;

                csettings.name_neg_palette = *(options.begin() + 1);
                break;

            //---------------------------------------------------------------------
            case cmdline_option::set_ppalette_filename:
                if(options.size() < 2)
                    return 2;

                csettings.name_pos_palette = *(options.begin() + 1);
                break;

            //---------------------------------------------------------------------
            case cmdline_option::set_coloring_mode:
            {   coloring_mode tmp_cmode = coloring_mode::unknown;
                if(options.size() < 2){
                    print_error("not enought arguments have been provided to set the coloring mode");
                    return 2;
                }

                const string tmp_cmode_str = *(options.begin() + 1);
                if(map_string_to_coloring_mode.contains(tmp_cmode_str))
                    tmp_cmode = map_string_to_coloring_mode.at(tmp_cmode_str);
                else{
                    print_error("unspecified/specified coloring mode is invalid");
                    return 2;
                }

                csettings.cmode = tmp_cmode;
            }   break;

            //---------------------------------------------------------------------
            case cmdline_option::enable_crosshair:
                csettings.draw_crosshair = true;
                break;

            //---------------------------------------------------------------------
            case cmdline_option::unknown:
            //default:   this is omitted so that if an option is not switched the compiler gives a warning
                print_error("unrecognized flag \"" + front_element + "\"");
                return -1;
                break;
        }

        //Pop the correct number of elements from the front of the vector
        options.erase(options.begin(), options.begin() + elements_to_pop);
    }

    return 0;
}