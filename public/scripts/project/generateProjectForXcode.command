#!/bin/bash

# This script should be run in ./project/ subdirectory
# each time source files are added or removed.

# Remove everything except of this script 
rm -rfv macOS
mkdir macOS
cd macOS

# Generate Xcode project 
cmake -G Xcode ./../../
