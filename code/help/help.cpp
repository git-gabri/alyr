#include "help.hpp"
void print_help(std::ostream& os){
    os <<
R"foo(
Another LYapunov fractal Renderer, made by git-gabri
HELP HAS TO BE FINISHED, THIS IS JUST A LIST OF FLAGS
-H
--help
-v
--verbose
-w
--width
-h
--height
-o
--output-image-filename
-S
--sector-size
-T
--max-threads
-m
--map
-s
-seq
--sequence
-xr
--x0-re
--real
-xi
--x0-im
--imag
-mra
--min-ra
-Mra
--max-ra
-mrb
--min-rb
-Mrb
--max-rb
-mrc
--min-rc
-Mrc
--max-rc
-t
--max-iter
-tt
--transient-iter
-lpc
--low-pos-clamp
-upc
--upp-pos-clamp
-lnc
--low-neg-clamp
-unc
--upp-neg-clamp
-np
--npalette-filename
-pp
--ppalette-filename
-c
--coloring-mode
-C
--crosshair
)foo";
}