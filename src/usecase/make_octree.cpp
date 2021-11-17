#include <iostream>
#include <laszip_api.h>
#include <vector>
#include <pcl_converter.h>

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

    std::ofstream grid_file;
    grid_file.open("grid_file.txt");
    for (int i = 0; i < grid_size; i++) {
        for (int j = 0; j < grid_size; j++) {
            for (int k = 0; k < grid_size; k++) {
                grid_file << grid[i * grid_size * grid_size + j * grid_size + k] << " ";
            }
            grid_file << "\n";
        }
    }
    grid_file.close();

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

    return 0;
}
