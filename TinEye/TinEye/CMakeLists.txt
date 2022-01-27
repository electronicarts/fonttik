# CMakeList.txt : CMake project for TinEye, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(Tesseract CONFIG REQUIRED)
include_directories(${Tesseract_INCLUDE_DIRS})

# Add source to this project's executable.
add_executable (TinEye "TinEye.cpp" "TinEye.h")

target_link_libraries(TinEye ${Tesseract_LIBRARIES})
# TODO: Add tests and install targets if needed.
