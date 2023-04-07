#!/bin/bash

rm -f Makefile
/home/lucas/builds/vulkanlinux-sdk/x86_64/bin/glslc ../include/shaders/triangle.vs -o vertex.spv
/home/lucas/builds/vulkanlinux-sdk/x86_64/bin/glslc ../include/shaders/triangle.fs -o fragment.spv
cmake -DCMAKE_BUILD_TYPE=Debug .. &&
make -j12 &&
./../bin/salamander
