#include <string>

void print_usage();

int info(std::string input_path);

int convert(std::string input_path, std::string output_path);

int split(std::string input_path, unsigned int chunk_size, std::string extension);

int make_octree(std::string input_path, unsigned int chunk_size, std::string extension);

int make_LOD(std::string input_path, unsigned int chunk_size, std::string extension);