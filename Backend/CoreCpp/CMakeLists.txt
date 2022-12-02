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

if($CACHE{CUDA})
	list(APPEND VCPKG_MANIFEST_FEATURES "cuda-opencv")
endif()
project ("Fonttik")

# Include sub-projects.
add_subdirectory ("Fonttik")

enable_testing()
add_subdirectory("Fonttik/FonttikLib.Tests")
add_subdirectory("Fonttik/AddressSanitizer.Tests")