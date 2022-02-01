# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
cmake_minimum_required (VERSION 3.8)

#project name
project ("TinEye")

#Source files for compilation
file(GLOB_RECURSE sources src/*.cpp src/*.h)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Tesseract CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

include_directories(${Tesseract_INCLUDE_DIRS})

# Add source to this project's executable.
add_executable (TinEye ${sources})

target_link_libraries(TinEye ${OpenCV_LIBS})
target_link_libraries(TinEye ${Tesseract_LIBRARIES})
target_link_libraries(TinEye nlohmann_json nlohmann_json::nlohmann_json)
# TODO: Add tests and install targets if needed.

#Copy data to destination folder
file(COPY data/config.json DESTINATION /)
file(COPY data/tessdata DESTINATION /)
file(COPY data/resources DESTINATION /)
#file(COPY ${tessdata} DESTINATION tessdata)
#file(COPY ${resources} DESTINATION resources)