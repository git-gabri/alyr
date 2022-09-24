#ifndef MAPS_HPP_INCLUDED
#define MAPS_HPP_INCLUDED

#include <complex>

template<typename T>
inline T logmap(const T& x, const T& r){
    return r * x * (T{1} - x);
}

template<typename T>
inline T logmap_der(const T& x, const T& r){
    return r * (T{1} - T{2}*x);
}

#endif