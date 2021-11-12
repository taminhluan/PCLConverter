#include <string>

void print_usage();

void info();

void print_points(unsigned int number_of_points);

void convert(std::string output_filepath);

void split(unsigned int chunk_size, std::string extension);

void make_octree(unsigned int chunk_size, std::string extension);

void make_LOD(unsigned int chunk_size, std::string extension);