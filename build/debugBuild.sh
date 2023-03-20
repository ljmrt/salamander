#!/bin/bash

rm -f Makefile
mkdir ../bin
cmake -DCMAKE_BUILD_TYPE=Debug .. &&
make -j12 &&
./../bin/salamander
