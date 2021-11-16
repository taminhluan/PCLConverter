# PCLConverter

Process point cloud data cross-platform application. convert format, split, merge, make octree, make level of detail

## Usage

PCLConverter -h: see how to use

PCLConverter -info *.las/.laz: print file info

PCLConverter -convert in.las/.laz/.txt/.pts output.las: convert to another format

PCLConverter -split in.las/.laz/.txt/.pts output.las chuck_size: split to many chunks

PCLConverter -octree in.las/.laz/.txt/.pts output.las chuck_size: make octree

PCLConverter -lod in.las/.laz/.txt/.pts output.las chuck_size: make LOD

## Error codes

-1: failed to load laszip.dll

## changelogs

### version 0.0.2

    - convert data
    - split data

### version 0.0.1

    - see information of .las/.laz
    - print n points
    - print bounding box

