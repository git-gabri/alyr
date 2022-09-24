#include "alyr.hpp"

#include <fstream>

using namespace std;
using namespace alyr::internals;

//Function to load the color palette from file
//Returns 0 if the loading was succesful, 1 if some warnings occurred, 2 if some errors occurred
/*The config file of the color palette should have the following format
* uint8 uint8 uint8
* uint8 uint8 uint8
* uint8 uint8 uint8
* [...]
*
* Each line represents a color to be loaded in the color palette, the first row corresponds
* to the RED channel, the middle row to the GREEN channel and the last row to the BLUE channel
*/
int alyr::load_palettes(){
    int ret_val = 0;

    //Clear color palettes
    npalette.clear();
    ppalette.clear();

    //------------------------------------------------------------------------------
    //Loading the negative color palette
    {
        //Open input text file
        ifstream input_file(csettings.name_neg_palette, ifstream::in);
        if(!input_file.is_open()){
            print_warning("negative palette could not be loaded correctly, defaulting to black/yellow");
            npalette.clear();
            npalette.push_back(png::rgb_pixel{0, 0, 0});        //Load black
            npalette.push_back(png::rgb_pixel{255, 255, 0});    //Load yellow
            if(ret_val < 1)
                ret_val = 1;
        }

        //Buffer to read the file line by line
        string line;
        while(getline(input_file, line)){
            //String stream to parse the line
            istringstream iss(line);
            //Temporary integers to store the values of the color, because the character 0 in the file gets interpreted as color 48,
            //so we have to store the reading from the file in an integer, so that it gets interpreted as a number
            int tmpred = 0, tmpgreen = 0, tmpblue = 0;

            //Read from iss and check that we read 3 color values. If not, return 2
            if(!(iss >> tmpred >> tmpgreen >> tmpblue)){
                print_error("formatting error in negative palette source file");
                ret_val = 2;
            }

            npalette.push_back(png::rgb_pixel(uint8_t(tmpred), uint8_t(tmpgreen), uint8_t(tmpblue)));
        }
    }

    //------------------------------------------------------------------------------
    //Loading the positive color palette
    {
        //Open input text file
        ifstream input_file(csettings.name_pos_palette, ifstream::in);
        if(!input_file.is_open()){
            print_warning("positive palette could not be loaded correctly, defaulting to black/blue");
            ppalette.clear();
            ppalette.push_back(png::rgb_pixel{0, 0, 0});        //Load black
            ppalette.push_back(png::rgb_pixel{0, 0, 255});      //Load blue
            if(ret_val < 1)
                ret_val = 1;
        }

        //Buffer to read the file line by line
        string line;
        while(getline(input_file, line)){
            //String stream to parse the line
            istringstream iss(line);
            //Temporary integers to store the values of the color, because the character 0 in the file gets interpreted as color 48,
            //so we have to store the reading from the file in an integer, so that it gets interpreted as a number
            int tmpred = 0, tmpgreen = 0, tmpblue = 0;

            //Read from iss and check that we read 3 color values. If not, return 2
            if(!(iss >> tmpred >> tmpgreen >> tmpblue)){
                print_error("formatting error in positive palette source file");
                ret_val = 2;
            }

            ppalette.push_back(png::rgb_pixel(uint8_t(tmpred), uint8_t(tmpgreen), uint8_t(tmpblue)));
        }
    }

    return ret_val;
}