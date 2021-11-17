#include <iostream>
#include <laszip_api.h>
#include <vector>
#include <pcl_converter.h>

void print_usage() {
    std::cout << "PCLConverter -h" << ": see how to use" << "\n";
    std::cout << "PCLConverter -info *.las/.laz" 
                <<": print file info"
                << "\n";

    std::cout << "PCLConverter -convert in.las/.laz/.txt/.pts output.las" 
                <<": convert to another format"
                << "\n";
                
    std::cout << "PCLConverter -split in.las/.laz/.txt/.pts output.las chuck_size" 
                <<": split to many chunks"
                << "\n";

    std::cout << "PCLConverter -octree in.las/.laz/.txt/.pts output.las chuck_size" 
                <<": make octree"
                << "\n";
    
    std::cout << "PCLConverter -lod in.las/.laz/.txt/.pts output.las chuck_size" 
                <<": make LOD"
                << "\n";
}

int main(int argc, char *argv[]) {
    std::cout << "PCL Converter" << " ";
    std::cout << argc << "\n";
    std::cout << argv[1] << "\n";

    // check dll
    {
        if (laszip_load_dll())
        {
            std::cout << "DLL ERROR: loading LASzip DLL\n";
            std::cout << "You should copy laszip.dll to binary folder (which contain pcl_converter.exe)";
            return -1;
        }
    }

    // std::cout << "argc " << argc <<  argv[0] ;
    if (argc > 1) {

        // "PCLConverter -h"
        if ( strcmp( argv[1], "-h") == 0 ) {
            std::cout << "in here";
            print_usage();
        } else if ( strcmp( argv[1], "-info") == 0 &&
                     argc > 1 ) { //"PCLConverter -info *.las/.laz"
            std::string input_path(argv[2]);
            info(input_path);
        } else if ( strcmp( argv[1], "-convert") == 0 && 
                    argc > 2 ) { //"PCLConverter -convert input.las output.laz" 
            std::string input_path(argv[2]);
            std::string output_path(argv[3]);
            convert(input_path, output_path);
        } else if ( strcmp( argv[1], "-split") == 0 &&
                    argc > 2 ) { //"PCLConverter -split input.las" 
            std::string input_path(argv[2]);
            split(input_path, 1000000, ".las");
        } else if ( strcmp( argv[1], "-octree") == 0 && 
                    argc > 2 ) { //"PCLConverter -octree input.las output.laz" 
            std::string input_path(argv[2]);
            make_octree(input_path, 1000, ".txt");
        } else if ( strcmp( argv[1], "-lod") == 0 &&
                    argc > 2 ) { //"PCLConverter -info input.las output.laz" 
            std::string input_path(argv[2]);
            make_octree(input_path, 1000, ".txt");
        } else {
            print_usage();
        }
    } else {
        print_usage();
    }

    return 0;
}