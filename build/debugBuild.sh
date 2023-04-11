#!/bin/bash

rm -f Makefile
/home/lucas/builds/vulkanlinux-sdk/x86_64/bin/glslc ../include/shaders/triangle.vert -o vertex.spv
/home/lucas/builds/vulkanlinux-sdk/x86_64/bin/glslc ../include/shaders/triangle.frag -o fragment.spv
cmake -DCMAKE_BUILD_TYPE=Debug .. &&
make -j12 &&
./../bin/salamander
