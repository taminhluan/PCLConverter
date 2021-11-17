cmake_minimum_required(VERSION 3.10)

project(PCLConverter)

add_executable(PCLConverter
    src/pcl_converter.cpp
    src/grid.cpp

    src/usecase/info.cpp
    src/usecase/convert.cpp
    src/usecase/make_lod.cpp
    src/usecase/make_octree.cpp
    src/usecase/split.cpp

    "${PROJECT_SOURCE_DIR}/external/LASzip/dll/laszip_api.c"
)


target_include_directories(PCLConverter
    PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/external/LASzip/dll"
    PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}/include"

)