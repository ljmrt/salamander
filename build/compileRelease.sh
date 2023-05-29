#!/bin/bash

echo
echo
echo "Note: Script must be run while in the \"build\" directory."
echo
echo "Compiling **release** build..."
echo
echo
rm -f Makefile
glslc ../include/shaders/scene.vert -o sceneVertex.spv
glslc ../include/shaders/scene.frag -o sceneFragment.spv
echo "../include/shaders/scene.vert --> sceneVertex.spv"
echo "../include/shaders/scene.frag --> sceneFragment.spv"
echo
echo
cmake -DCMAKE_BUILD_TYPE=Release .. &&
make -j12
echo "Done compiling, build can be found \"../bin/salamander\" if no errors occurred."
echo
echo
