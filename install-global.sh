#!/bin/bash
#Author: Michail Vourlakos
#Summary: Installation script for Now Dock Panel
#This script was written and tested on openSuSe Leap 42.1

set -e

if [ -d build ]; then
    cd build
    rm -fr *
else
    mkdir build
    cd build
fi

cmake -DCMAKE_INSTALL_PREFIX=/usr -DCMAKE_BUILD_TYPE=RELEASE ..
make
sudo make install
