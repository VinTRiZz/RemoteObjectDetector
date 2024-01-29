#! /bin/bash

CURRENT_DIR=$(pwd)

# Create dirs
mkdir ./BUILD &> /dev/null
mkdir ./BUILD/bin &> /dev/null
cd BUILD

# Configure
cmake .. &&

# Build
cmake --build .
