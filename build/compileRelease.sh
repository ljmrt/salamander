#!/bin/bash


p=$SALAMANDER_ROOT

echo
echo
echo "Compiling **release** build..."
echo
echo
rm -f Makefile
glslc $p/include/shaders/cubemap.vert -o cubemapVertex.spv
glslc $p/include/shaders/cubemap.frag -o cubemapFragment.spv
echo "$p/include/shaders/cubemap.vert --> cubemapVertex.spv"
echo "$p/include/shaders/cubemap.frag --> cubemapFragment.spv"
glslc $p/include/shaders/scene.vert -o sceneVertex.spv
glslc $p/include/shaders/scene.frag -o sceneFragment.spv
echo "$p/include/shaders/scene.vert --> sceneVertex.spv"
echo "$p/include/shaders/scene.frag --> sceneFragment.spv"
glslc $p/include/shaders/sceneNormals.vert -o sceneNormalsVertex.spv
glslc $p/include/shaders/sceneNormals.geom -o sceneNormalsGeometry.spv
glslc $p/include/shaders/sceneNormals.frag -o sceneNormalsFragment.spv
echo "$p/include/shaders/sceneNormals.vert --> sceneNormalsVertex.spv"
echo "$p/include/shaders/sceneNormals.geom --> sceneNormalsGeometry.spv"
echo "$p/include/shaders/sceneNormals.frag --> sceneNormalsFragment.spv"
glslc $p/include/shaders/directionalShadow.vert -o directionalShadowVertex.spv
glslc $p/include/shaders/directionalShadow.frag -o directionalShadowFragment.spv
echo "$p/include/shaders/directionalShadow.vert --> directionalShadowVertex.spv"
echo "$p/include/shaders/directionalShadow.frag --> directionalShadowFragment.spv"
glslc $p/include/shaders/pointShadow.vert -o pointShadowVertex.spv
glslc $p/include/shaders/pointShadow.frag -o pointShadowFragment.spv
echo "$p/include/shaders/pointShadow.vert --> pointShadowVertex.spv"
echo "$p/include/shaders/pointShadow.frag --> pointShadowFragment.spv"
echo
echo
cmake -DCMAKE_BUILD_TYPE=Release $p &&
make -j12
echo "Done compiling, build can be found \"$p/bin/salamander\" if no errors occurred."
echo
echo
