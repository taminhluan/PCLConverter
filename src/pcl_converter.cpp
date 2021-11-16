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

int info(std::string input_path) {
    std::cout << "GET INFO" << "\n";

    // create the reader
    laszip_POINTER laszip_reader;
    if ( laszip_create(&laszip_reader) ) {
        std::cerr << "DLL ERROR: creating laszip reader\n";
        return -1;
    }

    // open the reader
    laszip_BOOL is_compressed = 0;
    if ( laszip_open_reader(laszip_reader, input_path.c_str(), &is_compressed) ) {
        std::cerr << "DLL ERROR: open reader";
        return -1;
    }
    std::cout << "The input file is " << (is_compressed ? "compressed" : "uncompressed") << "\n";

    // get a pointer to the header of the reader that was just populated
    laszip_header* header;
    if (laszip_get_header_pointer(laszip_reader, &header)) {
        std::cerr << "DLL ERROR: getting header pointer from laszip header\n";
        return -1;
    }

    // print information
    laszip_I64 npoints = 0;
    {
        printf("%30s: %s\n", "file signature", "LASF");
        printf("%30s: %d\n", "file source ID", header->file_source_ID);
        printf("%30s: %d\n", "global encoding", header->global_encoding);
        printf("%30s: %d\n", "project ID GUID data 1", header->project_ID_GUID_data_1);
        printf("%30s: %d.%d\n", "verion major.minor", header->version_major, header->version_minor);
        printf("%30s: %s\n", "system identifier", header->system_identifier);
        printf("%30s: %s\n", "generating software", header->generating_software);
        printf("%30s: %d\n", "header size", header->header_size);
        printf("%30s: %d\n", "offset to point data", header->offset_to_point_data);
        printf("%30s: %f %f %f\n", "scale factor", header->x_scale_factor, header->y_scale_factor, header->z_scale_factor);
        printf("%30s: %f %f %f\n", "offset", header->x_offset, header->y_offset, header->z_offset);
        printf("%30s: %f %f %f\n", "min x y z", header->min_x, header->min_y, header->min_z);
        printf("%30s: %f %f %f\n", "max x y z", header->max_x, header->max_y, header->max_z);


        npoints = header->number_of_point_records ? header->number_of_point_records : header->extended_number_of_point_records;
        std::cout << "Number of point records: " << npoints << "\n";
    }

    std::cout << "PRINT " << npoints << " first POINTS\n";

    laszip_I64 p_count = 0;
    laszip_point* point;
    if (laszip_get_point_pointer(laszip_reader, &point)) {
        std::cerr << "DLL ERROR: get point pointer\n";
    }
    while (p_count < npoints) {
        // read a points
        if (laszip_read_point(laszip_reader)) {
            std::cerr << "DLL ERROR: reading points\n";
            return -1;
        }
        printf("%8d %8d %8d %8d %8d %8d\n", point->X, point->Y, point->Z, point->rgb[0], point->rgb[1], point->rgb[2]);

        p_count++;
    }


    // close the reader

    if (laszip_close_reader(laszip_reader))
    {
        fprintf(stderr, "DLL ERROR: closing laszip reader\n");
        return -1;
    }

    // destroy the reader

    if (laszip_destroy(laszip_reader))
    {
        fprintf(stderr, "DLL ERROR: destroying laszip reader\n");
        return -1;
    }

    return 0;
}


int convert(std::string input_path, std::string output_path) {
    std::cout << "CONVERT\n";
    std::cout << input_path <<"\n";
    std::cout << output_path << "\n";
    
    // create the reader
    laszip_POINTER laszip_reader;
    if (laszip_create(&laszip_reader)) {
        std::cerr << "DLL ERROR: creating laszip reader\n";
        return -1;
    }

    // open the reader
    laszip_BOOL is_compressed = 0;
    if (laszip_open_reader(laszip_reader, input_path.c_str(), &is_compressed)) {
        std::cerr << "DLL ERROR: open reader";
        return -1;
    }
    std::cout << "The input file is " << (is_compressed ? "compressed" : "uncompressed") << "\n";
    

    // get a pointer to the header of the reader that was just populated
    laszip_header* header;
    if (laszip_get_header_pointer(laszip_reader, &header)) {
        std::cerr << "DLL ERROR: getting header pointer from laszip header\n";
        return -1;
    }
    laszip_I64 npoints = (header->number_of_point_records ? header->number_of_point_records : header->extended_number_of_point_records);

    laszip_point* point;
    if (laszip_get_point_pointer(laszip_reader, &point))
    {
        fprintf(stderr, "DLL ERROR: getting point pointer from laszip reader\n");
        return -1;
    }

    // create the writer
    laszip_POINTER laszip_writer;
    if (laszip_create(&laszip_writer))
    {
        fprintf(stderr, "DLL ERROR: creating laszip writer\n");
        return -1;
    }

    // initialize the header for the writer using the header of the reader

    if (laszip_set_header(laszip_writer, header))
    {
        fprintf(stderr, "DLL ERROR: setting header for laszip writer\n");
        return -1;
    }

    // open the writer
    laszip_BOOL compress = (strstr(output_path.c_str(), ".laz") != 0);
    if (laszip_open_writer(laszip_writer, output_path.c_str(), compress))
    {
        fprintf(stderr, "DLL ERROR: opening laszip writer for '%s'\n", output_path.c_str());
        return -1;
    }
    fprintf(stderr, "writing file '%s' %scompressed\n", output_path.c_str(), (compress ? "" : "un"));

    laszip_I64 p_count = 0;

    while (p_count < npoints)
    {
        // read a point

        if (laszip_read_point(laszip_reader))
        {
            fprintf(stderr, "DLL ERROR: reading point %I64d\n", p_count);
            return -1;
        }

        // copy the point

        if (laszip_set_point(laszip_writer, point))
        {
            fprintf(stderr, "DLL ERROR: setting point %I64d\n", p_count);
            return -1;
        }

        // write the point

        if (laszip_write_point(laszip_writer))
        {
            fprintf(stderr, "DLL ERROR: writing point %I64d\n", p_count);
            return -1;
        }

        p_count++;
    }

    // close the writer

    if (laszip_close_writer(laszip_writer))
    {
        fprintf(stderr, "DLL ERROR: closing laszip writer\n");
        return -1;
    }

    // destroy the writer

    if (laszip_destroy(laszip_writer))
    {
        fprintf(stderr, "DLL ERROR: destroying laszip writer\n");
        return -1;
    }

    // close the reader

    if (laszip_close_reader(laszip_reader))
    {
        fprintf(stderr, "DLL ERROR: closing laszip reader\n");
        return -1;
    }

    // destroy the reader

    if (laszip_destroy(laszip_reader))
    {
        fprintf(stderr, "DLL ERROR: destroying laszip reader\n");
        return -1;
    }

    return 0;
}

int split(std::string input_path, unsigned int chunk_size, std::string extension) {
    std::cout << "SPLIT\n";
    std::cout << input_path <<"\n";
    std::cout << chunk_size << "\n";
    std::cout << extension << "\n";

    // create the reader
    laszip_POINTER laszip_reader;
    if (laszip_create(&laszip_reader)) {
        std::cerr << "DLL ERROR: creating laszip reader\n";
        return -1;
    }

    // open the reader
    laszip_BOOL is_compressed = 0;
    if (laszip_open_reader(laszip_reader, input_path.c_str(), &is_compressed)) {
        std::cerr << "DLL ERROR: open reader";
        return -1;
    }
    std::cout << "The input file is " << (is_compressed ? "compressed" : "uncompressed") << "\n";
    

    // get a pointer to the header of the reader that was just populated
    laszip_header* header;
    if (laszip_get_header_pointer(laszip_reader, &header)) {
        std::cerr << "DLL ERROR: getting header pointer from laszip header\n";
        return -1;
    }
    laszip_I64 npoints = (header->number_of_point_records ? header->number_of_point_records : header->extended_number_of_point_records);

    laszip_point* point;
    if (laszip_get_point_pointer(laszip_reader, &point))
    {
        fprintf(stderr, "DLL ERROR: getting point pointer from laszip reader\n");
        return -1;
    }

    

    laszip_I64 p_count = 0;
    laszip_I64 chunk_count = 0;

    laszip_POINTER laszip_writer;
    while (p_count < npoints)
    {
        if (p_count % 1000000 == 0) {
            std::cout << "Processed " << p_count << " points\n";
        }
        if ( p_count % chunk_size == 0 ) {
            if (p_count != 0) { // close writer
                chunk_count += 1;
                if (laszip_close_writer(laszip_writer))
                {
                    fprintf(stderr, "DLL ERROR: closing laszip writer\n");
                    return -1;
                }
            }

            
            { // open new writer
                // create the writer
                char output_file[11];
                sprintf(output_file, "%06d.las\0", chunk_count);
                if (laszip_create(&laszip_writer))
                {
                    fprintf(stderr, "DLL ERROR: creating laszip writer\n");
                    return -1;
                }

                // initialize the header for the writer using the header of the reader

                if (laszip_set_header(laszip_writer, header))
                {
                    fprintf(stderr, "DLL ERROR: setting header for laszip writer\n");
                    return -1;
                }

                // open the writer
                laszip_BOOL compress = (strstr(output_file, ".laz") != 0);

                if (laszip_open_writer(laszip_writer, output_file, compress))
                {
                    fprintf(stderr, "DLL ERROR: opening laszip writer for '%s'\n", output_file);
                    return -1;
                }
                fprintf(stderr, "writing file '%s' %scompressed\n", output_file, (compress ? "" : "un"));
            }
        }
        // read a point

        if (laszip_read_point(laszip_reader))
        {
            fprintf(stderr, "DLL ERROR: reading point %I64d\n", p_count);
            return -1;
        }

        // copy the point

        if (laszip_set_point(laszip_writer, point))
        {
            fprintf(stderr, "DLL ERROR: setting point %I64d\n", p_count);
            return -1;
        }

        // write the point

        if (laszip_write_point(laszip_writer))
        {
            fprintf(stderr, "DLL ERROR: writing point %I64d\n", p_count);
            return -1;
        }

        p_count++;
    }

    // close the writer

    if (laszip_close_writer(laszip_writer))
    {
        fprintf(stderr, "DLL ERROR: closing laszip writer\n");
        return -1;
    }

    // destroy the writer

    if (laszip_destroy(laszip_writer))
    {
        fprintf(stderr, "DLL ERROR: destroying laszip writer\n");
        return -1;
    }

    // close the reader

    if (laszip_close_reader(laszip_reader))
    {
        fprintf(stderr, "DLL ERROR: closing laszip reader\n");
        return -1;
    }

    // destroy the reader

    if (laszip_destroy(laszip_reader))
    {
        fprintf(stderr, "DLL ERROR: destroying laszip reader\n");
        return -1;
    }

    return 0;
}

int make_octree(std::string input_path, unsigned int chunk_size, std::string extension) {
    std::cout << "MAKE OCTREE\n";
    std::cout << input_path <<"\n";
    std::cout << chunk_size << "\n";
    std::cout << extension << "\n";

    // create the reader
    laszip_POINTER laszip_reader;
    if (laszip_create(&laszip_reader)) {
        std::cerr << "DLL ERROR: creating laszip reader\n";
        return -1;
    }

    // open the reader
    laszip_BOOL is_compressed = 0;
    if (laszip_open_reader(laszip_reader, input_path.c_str(), &is_compressed)) {
        std::cerr << "DLL ERROR: open reader";
        return -1;
    }
    std::cout << "The input file is " << (is_compressed ? "compressed" : "uncompressed") << "\n";

    // get a pointer to the header of the reader that was just populated
    laszip_header* header;
    if (laszip_get_header_pointer(laszip_reader, &header)) {
        std::cerr << "DLL ERROR: getting header pointer from laszip header\n";
        return -1;
    }

    // print information
    laszip_I64 npoints = 0;
    {
        printf("%30s: %s\n", "file signature", "LASF");
        printf("%30s: %d\n", "file source ID", header->file_source_ID);
        printf("%30s: %d\n", "global encoding", header->global_encoding);
        printf("%30s: %d\n", "project ID GUID data 1", header->project_ID_GUID_data_1);
        printf("%30s: %d.%d\n", "verion major.minor", header->version_major, header->version_minor);
        printf("%30s: %s\n", "system identifier", header->system_identifier);
        printf("%30s: %s\n", "generating software", header->generating_software);
        printf("%30s: %d\n", "header size", header->header_size);
        printf("%30s: %d\n", "offset to point data", header->offset_to_point_data);
        printf("%30s: %f %f %f\n", "scale factor", header->x_scale_factor, header->y_scale_factor, header->z_scale_factor);
        printf("%30s: %f %f %f\n", "offset", header->x_offset, header->y_offset, header->z_offset);
        printf("%30s: %f %f %f\n", "min x y z", header->min_x, header->min_y, header->min_z);
        printf("%30s: %f %f %f\n", "max x y z", header->max_x, header->max_y, header->max_z);


        npoints = header->number_of_point_records ? header->number_of_point_records : header->extended_number_of_point_records;
        std::cout << "Number of point records: " << npoints << "\n";
    }

    unsigned int npoints_per_chunk = 1000000;
    unsigned int npoints_per_node = 10000;
    unsigned int grid_size = 0;

    if (npoints < 10000000) {
        grid_size = 32;
    }
    else if (npoints < 100000000) {
        grid_size = 32; //128
    }
    else if (npoints < 500000000) {
        grid_size = 256;
    }
    else {
        grid_size = 512;
    }
    std::cout << "USE GRID SIZE: " << grid_size;

    std::vector<unsigned int> grid(grid_size * grid_size * grid_size);
       
    double cube_size = 0;

    { // calculate cube_size
        cube_size = header->max_x - header->min_x;

        if (cube_size < header->max_y - header->min_y) {
            cube_size = header->max_y - header->min_y;
        }

        if (cube_size < header->max_z - header->min_z) {
            cube_size = header->max_z - header->min_z;
        }
    }
    


    laszip_I64 p_count = 0;
    laszip_point* point;
    if (laszip_get_point_pointer(laszip_reader, &point)) {
        std::cerr << "DLL ERROR: get point pointer\n";
    }
    while (p_count < npoints) {
        // read a points
        if (laszip_read_point(laszip_reader)) {
            std::cerr << "DLL ERROR: reading points\n";
            return -1;
        }
        //printf("%8d %8d %8d %8d %8d %8d\n", point->X, point->Y, point->Z, point->rgb[0], point->rgb[1], point->rgb[2]);
        // real_coordinate = point * scale + offset
        
        double real_coordinate_x = point->X * header->x_scale_factor + header->x_offset;
        double real_coordinate_y = point->Y * header->y_scale_factor + header->y_offset;
        double real_coordinate_z = point->Z * header->z_scale_factor + header->z_offset;

        double cube_coordinate_x = real_coordinate_x - header->min_x;
        double cube_coordinate_y = real_coordinate_y - header->min_y;
        double cube_coordinate_z = real_coordinate_z - header->min_z;

        unsigned int grid_coordinate_x = (int) cube_coordinate_x / (cube_size / grid_size);
        unsigned int grid_coordinate_y = (int) cube_coordinate_y / (cube_size / grid_size);
        unsigned int grid_coordinate_z = (int) cube_coordinate_z / (cube_size / grid_size);

        //std::cout << grid_coordinate_x << "|" << grid_coordinate_y << "|" << grid_coordinate_z << "|\n";
        grid[grid_coordinate_x * grid_size * grid_size + grid_coordinate_y * grid_size + grid_coordinate_z] += 1;
        //std::cout << "Increased GRID"<< grid_coordinate_x << "," << grid_coordinate_y << "," << grid_coordinate_z << ":" << grid[grid_coordinate_x * grid_size * grid_size + grid_coordinate_y * grid_size + grid_coordinate_z];
        p_count++;
    }


    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            for (int k = 0; k < grid_size; k++) {
                std::cout << grid[i * grid_size * grid_size + j * grid_size + k] << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n\n";
    }

    // close the reader

    if (laszip_close_reader(laszip_reader))
    {
        fprintf(stderr, "DLL ERROR: closing laszip reader\n");
        return -1;
    }

    // destroy the reader

    if (laszip_destroy(laszip_reader))
    {
        fprintf(stderr, "DLL ERROR: destroying laszip reader\n");
        return -1;
    }

    std::cout << "PRINT " << npoints << " first POINTS\n";

    if ( npoints )

    return 0;
}

int make_LOD(std::string input_path, unsigned int chunk_size, std::string extension) {
    std::cout << "MAKE LOD\n";
    std::cout << input_path <<"\n";
    std::cout << chunk_size << "\n";
    std::cout << extension << "\n";
    return 0;
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
                    argc > 2 ) { //"PCLConverter -info input.las output.laz" 
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