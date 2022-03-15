# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.8)

# if (UNIX)
#    add_compile_definitions(_UNIX)
# endif (UNIX)

# if (MSVC)
#    add_compile_definitions(_WIN)
# endif (MSVC)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

project ("TinEye")

# Include sub-projects.
add_subdirectory ("TinEye")

enable_testing()
add_subdirectory("TinEye/TinEyeLib.Tests")
add_subdirectory("TinEye/AddressSanitizer.Tests")