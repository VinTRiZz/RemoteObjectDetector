#! /bin/bash

SCRIPT_DIR=$(dirname $(realpath "$0"))

# Create dirs
cd $SCRIPT_DIR
mkdir "$SCRIPT_DIR"/BUILD &> /dev/null
cd "$SCRIPT_DIR"/BUILD

# Configure
cmake .. &&

# Build
cmake --build .
