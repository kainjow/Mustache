#!/bin/bash

rm -rf _build
mkdir _build
cd _build
cmake ..
sudo cmake --build . --config Release --target install -- -j $(nproc)
# echo "Building $1"
# make -j4 $1
# sudo make install