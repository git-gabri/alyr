#include "help.hpp"
void print_help(std::ostream& os){
    os <<
R"foo(
Another LYapunov fractal Renderer, made by git-gabri

Disclaimer: program is still a work in progress, might be buggy, some features have not been implemented yet.

    Console output related flags
        -H
        --help
                    Print this help message and exit.

        -v
        --verbose
                    Print miscellaneous information on the current render during the execution.
                    This option is recommended but not on by default.

    Image related flags
        -w <SIZE_T>
        --width <SIZE_T>
                    Sets width of the image.

        -h <SIZE_T>
        --height <SIZE_T>
                    Sets height of the image.

        -o <STRING>
        --output-image-filename <STRING>
                    Sets the filename of the output image

    Rendering related flags
        -sm <STRING>
        --save <STRING>
        --save-matrix <STRING>
                    Saves the matrix of the calculated Lyapunov exponents to a file
                    whose name is "<STRING>.expbin".

        -lm <STRING>
        --load <STRING>
        --load-matrix <STRING>
                    Loads the matrix of the calculated Lyapunov exponents from a file
                    whose name is "<STRING.expbin>".
                    If this flag is specified, no calculations are performed and the
                    matrix is directly loaded.
                    This overwrites the image size with the size of the loaded matrix.

        --skip
        --skip-coloring
                    Skips the coloring of the image. The fractal image returned if this
                    flag is specified is a 1x1  black image.

        -S <SIZE_T>
        --sector-size <SIZE_T>
                    To use multithreading, this program divides the image into square
                    sectors which are at maximum <SIZE_T>x<SIZE_T> pixels^2.
                    This flag allows setting the maximum length of the side of these sectors.
                    The default value is 64.

        -T <SIZE_T>
        --max-threads <SIZE_T>
                    Maximum number of threads utilized to render the fractal.
                    By default this is automatically set to the maximum number of processing
                    core that the machine has.
                    If this detection fails, only 1 rendering thread is used.

    Fractal related flags
        -m <STRING>
        --map <STRING>
                    Sets the map to be used for the current render.
                    The supported maps are:
                    logmap          -> logistic map
                    circmap         -> circle map (not yet implemented)
                    gaussmap        -> Gauss map (not yet implemented)
                    custom          -> Custom map specified by the user with 2 other flags
                    The default value is "logmap"

        -s <STRING>
        -seq <STRING>
        --sequence <STRING>
                    Sets the order in which the different r values are used in the render.
                    STRING can contain only the characters 'A', 'B' and 'C'. No spaces.
                    The default value is "AB".
                    Note: 'C' currently is not yet implemented.

        -xr <DOUBLE>
        --x0-re <DOUBLE>
        --real <DOUBLE>
                    Sets the initial value of the real part of x to use in the renders.
                    The default value is 0.5.

        -xi <DOUBLE>
        --x0-im <DOUBLE>
        --imag <DOUBLE>
                    Sets the initial value of the imaginary part of x to use in the renders.
                    The default value is 0.

        NOTE:
        The next set of flags specifies the lower and upper boundaries of the values ra, rb and rc.
        A single image is created by iterating from the minimum to the maximum of ra and rb in a number
        of steps equal to the number of pixels in the height and width of the image, respectively.
        The values of ra are mapped to the y axis and the values of rb are mapped to the x axis.

        -mra <DOUBLE>
        --min-ra <DOUBLE>
                    Sets the minimum value of ra.
                    The default value is 0.
        -Mra <DOUBLE>
        --max-ra <DOUBLE>
                    Sets the maximum value of ra.
                    The default value is 4.
        -mrb <DOUBLE>
        --min-rb <DOUBLE>
                    Sets the minimum value of rb.
                    The default value is 0.
        -Mrb <DOUBLE>
        --max-rb <DOUBLE>
                    Sets the maximum value of rb.
                    The default value is 4.
        -mrc <DOUBLE>
        --min-rc <DOUBLE>
                    Sets the minimum value of rc.
                    The default value is 0.
                    NOTE: This feature is not yet implemented.
        -Mrc <DOUBLE>
        --max-rc <DOUBLE>
                    Sets the maximum value of rc.
                    The default value is 0.
                    NOTE: This feature is not yet implemented.

        -t <SIZE_T>
        --max-iter <SIZE_T>
                    Sets the maximum number of iterations of the map to perform for every pixel.
                    The default value is 2000.
        -tt <SIZE_T>
        --transient-iter <SIZE_T>
                    Sets the maximum number of initial iterations to ignore in the calculation of
                    the Lyapunov exponent. This is to remove the transient behaviour.
                    The default value is 200.

        NOTE:
        The next set of flags specifies the minima and the maxima of the positive and negative (finite) exponents.
        If an exponent falls outside of these two ranges, its value gets clamped to the minimum or the maximum of the
        positive or negative interval.

        -lpc <DOUBLE>
        --low-pos-clamp <DOUBLE>
                    Sets the lower clamping value for positive exponents.
                    The default value is 0.
        -upc <DOUBLE>
        --upp-pos-clamp <DOUBLE>
                    Sets the upper clamping value for positive exponents.
                    The default value is 10000.
        -lnc <DOUBLE>
        --low-neg-clamp <DOUBLE>
                    Sets the lower clamping value for negative exponents.
                    The default value is -10000.
        -unc <DOUBLE>
        --upp-neg-clamp <DOUBLE>
                    Sets the upper clamping value for negative exponents.
                    The default value is 0.

    Color related flags
        -np <STRING>
        --npalette-filename <STRING>
                    Specifies the filename of the palette for negative exponents.
                    The file has to be a text file, with a color on each line, specified in the format:
                    <NUMBER> <NUMBER> <NUMBER>
                    <NUMBER> <NUMBER> <NUMBER>
                    <NUMBER> <NUMBER> <NUMBER>
                    ...

                    Each <NUMBER> has to be between 0 and 255. The first number specifies the red component
                    of the color, the second the green component and the third the blue component.
                    By default the program looks for a file called "npalette". If this file is not found the
                    program fills the negative palette with black (0 0 0) and yellow (255 255 0).

        -pp <STRING>
        --ppalette-filename <STRING>
                    Specifies the filename of the palette for positive exponents.
                    The file has to be a text file, with a color on each line, specified in the format:
                    <NUMBER> <NUMBER> <NUMBER>
                    <NUMBER> <NUMBER> <NUMBER>
                    <NUMBER> <NUMBER> <NUMBER>
                    ...

                    Each <NUMBER> has to be between 0 and 255. The first number specifies the red component
                    of the color, the second the green component and the third the blue component.
                    By default the program looks for a file called "ppalette". If this file is not found the
                    program fills the negative palette with black (0 0 0) and blue (0 0 255).

        -c <STRING>
        --coloring-mode <STRING>
                    Sets the coloring mode to be used for the current render.
                    The supported coloring modes are:
                    binary          -> if the exponent is negative, the last color from the negative palette
                                       is chosen, otherwise the last color from the positive palette is chosen.
                    linear          -> maps the palettes to the maxima and minima of the positive and negative
                                       exponents and performs linear interpolation between colors.
                    The default value is "linear"

        -C
        --crosshair
                    Draws a crosshair in the middle of the image.
)foo";
}