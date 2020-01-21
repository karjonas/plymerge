# plymerge

A simple tool for merging a list of ascii ply files.

## Compile
```
mkdir build
cd build
cmake ..
make
```

## Usage
Create a file named 'inputfile' containing the .ply files to merge on separate lines, the run:
```
./plymerge inputfile output.ply
```
