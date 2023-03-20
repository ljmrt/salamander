#!/bin/bash

rm -f Makefile
mkdir ../bin
cmake -DCMAKE_BUILD_TYPE=Release .. &&
make -j12 &&
./../bin/salamander
