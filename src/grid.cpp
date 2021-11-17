#include <iostream>
#include <fstream>
#include <grid.h>

void import_grid(std::vector<unsigned int> &grid, unsigned int grid_size, char* input_file) {
    std::ifstream grid_istream;
    grid_istream.open(input_file);

    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            for (int k = 0; k < grid_size; k++) {
                unsigned int cell_value;
                grid_istream >> cell_value;

                grid[i * grid_size * grid_size + j * grid_size + k] = cell_value;
            }
        }
    }
    
    grid_istream.close();
}
void export_grid(std::vector<unsigned int> &grid, unsigned int grid_size, char* input_file) {
    std::ofstream grid_ostream;
    grid_ostream.open(input_file);

    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            for (int k = 0; k < grid_size; k++) {
                unsigned int grid_cell_value = grid[i * grid_size * grid_size + j * grid_size + k];
                grid_ostream << grid_cell_value << " ";
            }
            grid_ostream << "\n";
        }
    }

    grid_ostream.close();
}