#include "alyr.hpp"

#include <fstream>

//Save Lyapunov exponent matrix to file
int alyr::internals::save_lyap_exp_matrix(const std::vector<std::vector<long double>>& matr, const std::string& filename){
    std::ofstream out_file(filename + ".expbin", std::ios::out | std::ios::binary);

    if(!out_file.is_open()){
        print_error("couldn't open exponent matrix file for saving");
        return 1;
    }

    //Dimensions of the matrix
    const size_t num_rows = matr.size();
    const size_t num_cols = matr.front().size();

    //Dimension of the single element of the matrix
    const size_t size_single_element = sizeof(long double);

    //Start writing data to file
    out_file.write(reinterpret_cast<const char*>(&num_rows), sizeof(num_rows));
    out_file.write(reinterpret_cast<const char*>(&num_cols), sizeof(num_cols));
    out_file.write(reinterpret_cast<const char*>(&size_single_element), sizeof(size_single_element));

    //Write entire matrix to file, row by row
    for(size_t y = 0; y < num_rows; ++y){
        for(size_t x = 0; x < num_cols; ++x){
            out_file.write(reinterpret_cast<const char*>(&matr[y][x]), size_single_element);
        }
    }

    //Close the file
    out_file.close();

    return 0;
}

//Load Lyapunov exponent matrix from file
std::vector<std::vector<long double>> alyr::internals::load_lyap_exp_matrix(const std::string& filename){
    std::vector<std::vector<long double>> ret_matr;

    std::ifstream in_file(filename + ".expbin", std::ios::in | std::ios::ate | std::ios::binary);

    if(!in_file.is_open())
        print_error("couldn't open exponent matrix file for loading");
    else{
        const size_t file_size = in_file.tellg();
        in_file.seekg(std::ios::beg);

        //First checks on size of the file
        if(file_size < 3 * sizeof(size_t))
            print_error("couldn't load header from exponent matrix file");
        else{
            size_t num_rows = 0;
            size_t num_cols = 0;
            size_t size_single_element = 0;

            //Read header
            in_file.read(reinterpret_cast<char*>(&num_rows), sizeof(size_t));
            in_file.read(reinterpret_cast<char*>(&num_cols), sizeof(size_t));
            in_file.read(reinterpret_cast<char*>(&size_single_element), sizeof(size_t));

            //Other checks on size of the file
            if(file_size != 3 * sizeof(size_t) + num_rows*num_cols*size_single_element)
                print_error("couldn't load exponent matrix file, size is invalid");
            else{
                ret_matr = std::vector<std::vector<long double>>(num_rows, std::vector<long double>(num_cols, 0));

                //Write entire matrix to file, row by row
                for(size_t y = 0; y < num_rows; ++y){
                    for(size_t x = 0; x < num_cols; ++x){
                        in_file.read(reinterpret_cast<char*>(&ret_matr[y][x]), size_single_element);
                    }
                }
            }
        }
    }

    return ret_matr;
}