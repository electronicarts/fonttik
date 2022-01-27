# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
cmake_minimum_required (VERSION 3.8)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project ("TinEye")

# Include sub-projects.
add_subdirectory ("TinEye")
