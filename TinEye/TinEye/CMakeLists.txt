# CMakeList.txt : CMake project for TinEye, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

find_package(Tesseract CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

include_directories(${Tesseract_INCLUDE_DIRS})

# Add source to this project's executable.
add_executable (TinEye "TinEye.cpp" "TinEye.h" "Configuration.h")

target_link_libraries(TinEye ${OpenCV_LIBS})
target_link_libraries(TinEye ${Tesseract_LIBRARIES})
target_link_libraries(TinEye nlohmann_json nlohmann_json::nlohmann_json)
# TODO: Add tests and install targets if needed.
