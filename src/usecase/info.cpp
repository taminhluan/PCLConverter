#include <iostream>
#include <laszip_api.h>
#include <vector>
#include <pcl_converter.h>

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