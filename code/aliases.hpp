#ifndef ALIASES_HPP_INCLUDED
#define ALIASES_HPP_INCLUDED

#include <complex>
#include <vector>
#include <png++/png.hpp>

using map_fn_ptr_t =
    std::complex<long double> (*)(const std::complex<long double>& x,
                                  const std::complex<long double>& r);
using map_der_fn_ptr_t =
    std::complex<long double> (*)(const std::complex<long double>& x,
                                  const std::complex<long double>& r);
using block_exp_calc_fn_ptr_t =
    void (*)(const size_t& img_widht,   const size_t& img_height,
             const size_t& start_x,     const size_t& start_y,
             const size_t& end_x,       const size_t& end_y,
             std::vector<std::vector<long double>>& lyap_exp_matr);

using block_renderer_fn_ptr_t =
    void (*)(const size_t& start_x,      const size_t& start_y,
             const size_t& end_x,        const size_t& end_y,
             const long double& max_pos, const long double& min_neg,
             std::vector<std::vector<long double>>& lyap_exp_matr,
             png::image<png::rgb_pixel>& image_to_color);
#endif