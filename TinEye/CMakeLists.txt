# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
cmake_minimum_required (VERSION 3.8)

#project name
project ("TinEye")

#Source files for compilation
file(GLOB_RECURSE sources src/*.cpp src/*.h)
file(GLOB_RECURSE test_sources src_tests/*.cpp src_tests/*.h)
file(GLOB_RECURSE east_src east_prototype/*.cpp east_prototype/*.h)
file(GLOB_RECURSE cli_sources src_cli/*.cpp src_cli/*.h)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

find_package(Tesseract CONFIG REQUIRED)
find_package(OpenCV CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(Boost REQUIRED COMPONENTS log)


include_directories(${Tesseract_INCLUDE_DIRS} ${Boost_INCLUDE_DIRS})
# Add source to this project's executable.
add_library (TinEye SHARED ${sources} )



add_executable(TinEye_CLI ${cli_sources} )

add_executable (EAST ${east_src} )

set(Tineye_Libs ${OpenCV_LIBS} 
${Tesseract_LIBRARIES} 
nlohmann_json nlohmann_json::nlohmann_json
${Boost_LIBRARIES})

target_link_libraries(TinEye PRIVATE ${Tineye_Libs})

target_link_libraries(TinEye_CLI TinEye)
target_include_directories(TinEye_CLI PRIVATE src/)

target_link_libraries(EAST ${Tineye_Libs})


#Unit tests
enable_testing()
find_package(GTest CONFIG REQUIRED)

add_executable(Tineye_tests ${test_sources} )

target_link_libraries(Tineye_tests GTest::gtest GTest::gtest_main ${Tineye_Libs})

target_include_directories(Tineye_tests PRIVATE src/)

add_test(Tineye_tests Tineye_tests)

#Copy data to destination folder
file(COPY data/config.json DESTINATION ./)
file(COPY data/tessdata DESTINATION ./)
file(COPY data/resources DESTINATION ./)
file(COPY data/unit_test DESTINATION ./)