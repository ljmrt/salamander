#!/bin/bash

rm -f Makefile
glslc ../include/shaders/triangle.vert -o vertex.spv
glslc ../include/shaders/triangle.frag -o fragment.spv
cmake -DCMAKE_BUILD_TYPE=Debug .. &&
make -j12 &&
./../bin/salamander
