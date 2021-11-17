#include <iostream>
#include <laszip_api.h>
#include <vector>
#include <pcl_converter.h>

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
