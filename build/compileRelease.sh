#!/bin/bash

echo
echo
echo "Note: Script must be run while in the \"build\" directory."
echo
echo "Compiling **release** build..."
echo
echo
rm -f Makefile
glslc ../include/shaders/triangle.vert -o vertex.spv
glslc ../include/shaders/triangle.frag -o fragment.spv
echo "../include/shaders/triangle.vert --> vertex.spv"
echo "../include/shaders/triangle.frag --> fragment.spv"
echo
echo
cmake -DCMAKE_BUILD_TYPE=Release .. &&
make -j12
echo "Done compiling, build can be found \"../bin/salamander\" if no errors occurred."
echo
echo
