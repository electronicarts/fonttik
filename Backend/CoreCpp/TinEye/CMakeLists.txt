# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
cmake_minimum_required (VERSION 3.8)

#Source files for compilation
file(GLOB_RECURSE test_sources src_tests/*.cpp src_tests/*.h)
file(GLOB_RECURSE cli_sources src_cli/*.cpp src_cli/*.h)

find_package(Tesseract CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS log)

set(TinEye_DEPS ${OpenCV_LIBS}
${Tesseract_LIBRARIES} 
nlohmann_json nlohmann_json::nlohmann_json
${Boost_LIBRARIES})

add_subdirectory("east_prototype")
add_subdirectory("TinEyeLib")
add_subdirectory("TinEyeApp")
