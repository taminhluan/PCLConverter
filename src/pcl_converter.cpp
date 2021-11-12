#include <iostream>
#include <laszip_api.h>

#include <pcl_converter.h>

void print_usage() {
    std::cout << "pcl_converter -h" << ": see how to use" << "\n";
    std::cout << "pcl_converter -info *.las/.laz" 
                <<": print file info"
                << "\n";

    std::cout << "pcl_converter -convert in.las/.laz/.txt/.pts output.las" 
                <<": convert to another format"
                << "\n";
                
    std::cout << "pcl_converter -split in.las/.laz/.txt/.pts output.las chuck_size" 
                <<": split to many chunks"
                << "\n";

    std::cout << "pcl_converter -octree in.las/.laz/.txt/.pts output.las chuck_size" 
                <<": make octree"
                << "\n";
    
    std::cout << "pcl_converter -lod in.las/.laz/.txt/.pts output.las chuck_size" 
                <<": make LOD"
                << "\n";
}

void info() {
    std::cout << "under constrution";
}

void print_points(unsigned int number_of_points) {
    std::cout << "under constrution";
}

void convert(std::string output_filepath) {
    std::cout << "under constrution";
}

void split(unsigned int chunk_size, std::string extension) {
    std::cout << "under constrution";
}

void make_octree(unsigned int chunk_size, std::string extension) {
    std::cout << "under constrution";
}

void make_LOD(unsigned int chunk_size, std::string extension) {
    std::cout << "under constrution";
}

int main() {
    std::cout << "PCL Converter";

    if (laszip_load_dll())
    {
        std::cout << "DLL ERROR: loading LASzip DLL\n";
        std::cout << "You should copy laszip.dll to binary folder (which contain pcl_converter.exe)";
    }


    return 0;
}