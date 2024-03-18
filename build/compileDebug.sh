#!/bin/bash


p=$SALAMANDER_ROOT

echo
echo
echo "Compiling **debug** build..."
echo
echo
rm -f Makefile
glslc $p/include/shaders/cubemap.vert -o $p/build/cubemapVertex.spv
glslc $p/include/shaders/cubemap.frag -o $p/build/cubemapFragment.spv
echo "$p/include/shaders/cubemap.vert --> $p/build/cubemapVertex.spv"
echo "$p/include/shaders/cubemap.frag --> $p/build/cubemapFragment.spv"
glslc $p/include/shaders/scene.vert -o $p/build/sceneVertex.spv
glslc $p/include/shaders/scene.frag -o $p/build/sceneFragment.spv
echo "$p/include/shaders/scene.vert --> $p/build/sceneVertex.spv"
echo "$p/include/shaders/scene.frag --> $p/build/sceneFragment.spv"
glslc $p/include/shaders/sceneNormals.vert -o $p/build/sceneNormalsVertex.spv
glslc $p/include/shaders/sceneNormals.geom -o $p/build/sceneNormalsGeometry.spv
glslc $p/include/shaders/sceneNormals.frag -o $p/build/sceneNormalsFragment.spv
echo "$p/include/shaders/sceneNormals.vert --> $p/build/sceneNormalsVertex.spv"
echo "$p/include/shaders/sceneNormals.geom --> $p/build/sceneNormalsGeometry.spv"
echo "$p/include/shaders/sceneNormals.frag --> $p/build/sceneNormalsFragment.spv"
glslc $p/include/shaders/directionalShadow.vert -o $p/build/directionalShadowVertex.spv
glslc $p/include/shaders/directionalShadow.frag -o $p/build/directionalShadowFragment.spv
echo "$p/include/shaders/directionalShadow.vert --> $p/build/directionalShadowVertex.spv"
echo "$p/include/shaders/directionalShadow.frag --> $p/build/directionalShadowFragment.spv"
glslc $p/include/shaders/pointShadow.vert -o $p/build/pointShadowVertex.spv
glslc $p/include/shaders/pointShadow.frag -o $p/build/pointShadowFragment.spv
echo "$p/include/shaders/pointShadow.vert --> $p/build/pointShadowVertex.spv"
echo "$p/include/shaders/pointShadow.frag --> $p/build/pointShadowFragment.spv"
echo
echo
cmake -DCMAKE_BUILD_TYPE=Debug $p/build &&
make -j12
echo "Done compiling, build can be found \"$p/bin/salamander\" if no errors occurred."
echo
echo
