#include <iostream>

#include "alyr.hpp"

int main(int argc, char** argv){
    //Initialize namespace
    alyr::init();

    //Parse command line options
    switch(alyr::parse_options(std::vector<std::string>(argv + 1, argv + argc))){
        //Success
        case 0:
            break;

        //Help was printed
        case 1:
            return EXIT_SUCCESS;
            break;

        //Anything else
        default:
            return EXIT_FAILURE;
            break;
    }

    //Load the palettes
    switch(alyr::load_palettes()){
        //Success
        case 0:
            break;

        //Warnings
        case 1:
            break;

        //Anything else
        default:
            return EXIT_FAILURE;
            break;
    }

    auto img = alyr::render();

    img.write(alyr::internals::isettings.image_name + ".png");

	return 0;
}
