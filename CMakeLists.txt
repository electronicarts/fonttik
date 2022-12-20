#----------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#----------------------------------------------------------------------------------------
cmake_minimum_required (VERSION 3.21)
project(Fonttik CXX)

# ---------------------------------------------------------------------------------------
# Options
# ---------------------------------------------------------------------------------------
option(BUILD_TESTS "Build library tests" OFF)
option(BUILD_APPS "Build Fonttik console application" OFF)
option(BUILD_BENCHMAKRS "Build Fonttik benchmarks" OFF)
option(EXPORT_FONTTIK "Export and install library" OFF)

# ---------------------------------------------------------------------------------------
# Compiler Config
# ---------------------------------------------------------------------------------------
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# ---------------------------------------------------------------------------------------
# CUDA feature
# ---------------------------------------------------------------------------------------
if($CACHE{CUDA})
	list(APPEND VCPKG_MANIFEST_FEATURES "cuda-opencv")
endif()

# ---------------------------------------------------------------------------------------
# Fonttik Library
# ---------------------------------------------------------------------------------------

# Define source files
file(GLOB_RECURSE HEADER_FILES ./include/fonttik/*.h)
file(GLOB_RECURSE PRIVATE_HEADER_FILES ./src/*.h)
file(GLOB_RECURSE SOURCE_FILES ./src/*.cpp)

# Dependencies
find_package(OpenCV CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)

# Library definition
add_library(fonttik STATIC ${SOURCE_FILES} ${PRIVATE_HEADER_FILES})

target_include_directories(fonttik
	PUBLIC
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
		"$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>" 
)

set_target_properties(fonttik PROPERTIES 
			PUBLIC_HEADER "${HEADER_FILES}"
)
target_link_libraries(fonttik PUBLIC
								${OpenCV_LIBS} 
								nlohmann_json nlohmann_json::nlohmann_json 
								spdlog::spdlog
)

# ---------------------------------------------------------------------------------------
# Export Target
# ---------------------------------------------------------------------------------------

if(EXPORT_FONTTIK)
	message("Install targets")
	install(TARGETS fonttik EXPORT FonttikTargets ARCHIVE
				PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/fonttik
	)

	install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/data
			DESTINATION ${CMAKE_INSTALL_LIBDIR}
			DESTINATION ${CMAKE_INSTALL_BINDIR}
	)

	install(EXPORT FonttikTargets
  		DESTINATION ${CMAKE_INSTALL_LIBDIR}/fonttik/cmake
  		NAMESPACE fonttik::
	)

	include(GNUInstallDirs)
	include(CMakePackageConfigHelpers)

	set(LIB_INSTALL_DIR ${CMAKE_INSTALL_LIBDIR}/fonttik)

	configure_package_config_file(	${CMAKE_CURRENT_SOURCE_DIR}/cmake/FonttikConfig.cmake.in
									"${CMAKE_CURRENT_BINARY_DIR}/FonttikConfig.cmake"
									INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/fonttik/cmake
									PATH_VARS LIB_INSTALL_DIR
	)

	install(FILES "${CMAKE_CURRENT_BINARY_DIR}/FonttikConfig.cmake"
					DESTINATION ${CMAKE_INSTALL_LIBDIR}/fonttik/cmake
	)

endif()


# ---------------------------------------------------------------------------------------
# Build Fonttik Apps
# ---------------------------------------------------------------------------------------

if(BUILD_APPS)
	message("Build Fonttik Apps")
	add_subdirectory ("apps/FonttikApp")
	add_subdirectory ("apps/FonttikApp_MT")
endif()

# ---------------------------------------------------------------------------------------
# Build Tests
# ---------------------------------------------------------------------------------------
if(BUILD_TESTS)
	message("Build tests")
	enable_testing()
	add_subdirectory("tests/Fonttik.Tests")
	add_subdirectory("tests/AddressSanitizer.Tests")
endif()

# ---------------------------------------------------------------------------------------
# Build Benchmarks
# ---------------------------------------------------------------------------------------
if(BUILD_BENCHMAKRS)
	message("Build benchmarks")
	add_subdirectory(benchmarks)
endif()