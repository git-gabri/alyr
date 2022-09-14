#include "alyr.hpp"

//Renderer of a certain region
void alyr::internals::block_renderer(const size_t& start_x,      const size_t& start_y,
                                     const size_t& end_x,        const size_t& end_y,
                                     const long double& max_pos, const long double& min_neg,
                                     std::vector<std::vector<long double>>& lyap_exp_matr,
                                     png::image<png::rgb_pixel>& img_to_color)
{
    //Iterate over all the pixels in the block
    for(size_t x = start_x; x < end_x; ++x){
        for(size_t y = start_y; y < end_y; ++y){
            //std::cout << "Computing for " << lyap_exp_matr[y][x] << std::endl;
            png::rgb_pixel current_pixel;

            switch(csettings.cmode){
                //Binary coloring
                default:
                case coloring_mode::binary:
                    if(lyap_exp_matr[y][x] >= 0)
                        current_pixel = ppalette.front();
                    else
                        current_pixel = npalette.front();
                    break;

                //Linear coloring
                case coloring_mode::linear: {
                    //Sign of the exponent
                    //1 -> negative
                    //0 -> positive
                    const bool exp_sign = (lyap_exp_matr[y][x] < 0);

                    //Filter out infinities
                    if(!std::isfinite(lyap_exp_matr[y][x])){
                        if(lyap_exp_matr[y][x] >= 0)
                            current_pixel = ppalette.back();
                        else
                            current_pixel = npalette.back();
                        break;
                    }

                    //Normalized exponent to [0, 1]
                    const long double normalized_exp = ((exp_sign == 0) ? lyap_exp_matr[y][x] / max_pos : lyap_exp_matr[y][x] / min_neg);
                    assert(normalized_exp >= 0 && normalized_exp <= 1);

                    //Color selection
                    const long double fractional_color = ((exp_sign == 0) ? normalized_exp * static_cast<long double>(ppalette.size()) :
                                                                            normalized_exp * static_cast<long double>(npalette.size()));
                    //Indexes of the color right after and right before the selected one,
                    //because (probably) fractional_color is not an integer
                    const size_t lower_color_id = size_t(floor(fractional_color));
                    const size_t upper_color_id = size_t( ceil(fractional_color)) % ((exp_sign == 0) ? ppalette.size() : npalette.size());

                    //"Percentage", fraction in [0, 1], representing how much to take from every color for linear interpolation
                    const long double lower_color_fraction = fractional_color - static_cast<long double>(lower_color_id);
                    const long double upper_color_fraction = fractional_color - static_cast<long double>(upper_color_id);

                    //Colors to blend
                    const png::rgb_pixel lower_color = ((exp_sign == 0) ? ppalette[lower_color_id] : npalette[lower_color_id]);
                    const png::rgb_pixel upper_color = ((exp_sign == 0) ? ppalette[upper_color_id] : npalette[upper_color_id]);

                    current_pixel = png::rgb_pixel(
                        static_cast<long double>(lower_color.red)   * lower_color_fraction + static_cast<long double>(upper_color.red)   * upper_color_fraction,
                        static_cast<long double>(lower_color.green) * lower_color_fraction + static_cast<long double>(upper_color.green) * upper_color_fraction,
                        static_cast<long double>(lower_color.blue)  * lower_color_fraction + static_cast<long double>(upper_color.blue)  * upper_color_fraction
                    );
                }   break;
            }

            //Actually color the image
            img_to_color[y][x] = current_pixel;
        }
    }
}