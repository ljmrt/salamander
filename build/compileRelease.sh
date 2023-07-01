#!/bin/bash

echo
echo
echo "Note: Script must be run while in the \"build\" directory."
echo
echo "Compiling **release** build..."
echo
echo
rm -f Makefile
glslc ../include/shaders/cubemap.vert -o cubemapVertex.spv
glslc ../include/shaders/cubemap.frag -o cubemapFragment.spv
echo "../include/shaders/cubemap.vert --> cubemapVertex.spv"
echo "../include/shaders/cubemap.frag --> cubemapFragment.spv"
glslc ../include/shaders/scene.vert -o sceneVertex.spv
glslc ../include/shaders/scene.frag -o sceneFragment.spv
echo "../include/shaders/scene.vert --> sceneVertex.spv"
echo "../include/shaders/scene.frag --> sceneFragment.spv"
glslc ../include/shaders/sceneNormals.vert -o sceneNormalsVertex.spv
glslc ../include/shaders/sceneNormals.geom -o sceneNormalsGeometry.spv
glslc ../include/shaders/sceneNormals.frag -o sceneNormalsFragment.spv
echo "../include/shaders/sceneNormals.vert --> sceneNormalsVertex.spv"
echo "../include/shaders/sceneNormals.geom --> sceneNormalsGeometry.spv"
echo "../include/shaders/sceneNormals.frag --> sceneNormalsFragment.spv"
glslc ../include/shaders/directionalShadow.vert -o directionalShadowVertex.spv
glslc ../include/shaders/directionalShadow.frag -o directionalShadowFragment.spv
echo "../include/shaders/directionalShadow.vert --> directionalShadowVertex.spv"
echo "../include/shaders/directionalShadow.frag --> directionalShadowFragment.spv"
glslc ../include/shaders/pointShadow.vert -o pointShadowVertex.spv
glslc ../include/shaders/pointShadow.geom -o pointShadowGeometry.spv
glslc ../include/shaders/pointShadow.frag -o pointShadowFragment.spv
echo "../include/shaders/pointShadow.vert --> pointShadowVertex.spv"
echo "../include/shaders/pointShadow.vert --> pointShadowGeometry.spv"
echo "../include/shaders/pointShadow.frag --> pointShadowFragment.spv"
echo
echo
cmake -DCMAKE_BUILD_TYPE=Release .. &&
make -j12
echo "Done compiling, build can be found \"../bin/salamander\" if no errors occurred."
echo
echo
