#include <string>

void print_usage();

void info(std::string input_path);

void print_points(unsigned int number_of_points);

void convert(std::string input_path, std::string output_path);

void split(std::string input_path, unsigned int chunk_size, std::string extension);

void make_octree(std::string input_path, unsigned int chunk_size, std::string extension);

void make_LOD(std::string input_path, unsigned int chunk_size, std::string extension);