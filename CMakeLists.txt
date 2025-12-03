#----------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#----------------------------------------------------------------------------------------
cmake_minimum_required (VERSION 3.21)
project(fonttik LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR}/cmake/)

# ---------------------------------------------------------------------------------------
# Options 
# ---------------------------------------------------------------------------------------
option(BUILD_TESTS "Build library tests" ON)
option(BUILD_EXAMPLE_APP "Build Fonttik example console application" ON)
option(BUILD_SHARED_LIBS "Build Fonttik as a shared library" OFF)
option(EXPORT_FONTTIK "Export and install library" OFF)
option(BUILD_COVERAGE "Builds code coverage target" OFF)

if (BUILD_COVERAGE AND NOT UNIX)
    set(BUILD_COVERAGE OFF)
    message("Code coverage can only be built in Linux systems")
endif()

# ---------------------------------------------------------------------------------------
# Compiler Config
# ---------------------------------------------------------------------------------------a

if(BUILD_COVERAGE)
    message("BUILD CODE COVERAGE")
    SET(GCC_COVERAGE_COMPILE_FLAGS "-g -O0 -coverage -fprofile-arcs -ftest-coverage")
    SET(GCC_COVERAGE_LINK_FLAGS    "-coverage -lgcov")
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
    SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
endif()

# ---------------------------------------------------------------------------------------
# Fonttik Library
# ---------------------------------------------------------------------------------------

# Define source files
set(PUBLIC_HEADERS
    "include/fonttik/Configuration.hpp"
    "include/fonttik/ConfigurationParams.hpp"
    "include/fonttik/Fonttik.hpp"
    "include/fonttik/Frame.hpp"
    "include/fonttik/Media.hpp"
    "include/fonttik/TextBox.hpp"
    "include/fonttik/Log.h"
    "include/fonttik/Results.h"
)

source_group("Public header files" FILES ${PUBLIC_HEADERS})

set(SOURCE_FILES
    "src/Configuration.cpp"
    "src/ColorblindFilters.cpp"
    "src/IChecker.h"
    "src/ITextboxDetection.h"
    "src/ITextboxDetection.cpp"
    "src/ITextboxRecognition.h"
    "src/Log.cpp"
    "src/ContrastChecker.cpp"
    "src/SizeChecker.cpp"
    "src/Fonttik.cpp"
    "src/Frame.cpp"
    "src/Image.cpp"
    "src/Video.cpp"
    "src/Media.cpp"
    "src/Textbox.cpp"
    "src/Results.cpp"
    "src/TextboxDetectionDB.h"
    "src/TextboxDetectionDB.cpp"
    "src/TextboxDetectionEAST.h"
    "src/TextboxDetectionEAST.cpp"
    "src/TextBoxRecognitionOpenCV.hpp"
    "src/TextboxRecognitionOpenCV.cpp"
)

source_group("Source files" FILES ${SOURCE_FILES}) 

# Dependencies
find_package(OpenCV CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(SPSCQueue CONFIG REQUIRED)

if(BUILD_SHARED_LIBS)
    message("BUILD SHARED LIBRARIES")
    if(WIN32)
        set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
        message("Export symbols")
    endif()
endif()

if(BUILD_COVERAGE)
    message("BUILD CODE COVERAGE")
    SET(GCC_COVERAGE_COMPILE_FLAGS "-g -O0 -coverage -fprofile-arcs -ftest-coverage")
    SET(GCC_COVERAGE_LINK_FLAGS    "-coverage -lgcov")
    SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
    SET( CMAKE_EXE_LINKER_FLAGS  "${CMAKE_EXE_LINKER_FLAGS} ${GCC_COVERAGE_LINK_FLAGS}" )
endif()


# Library definition
add_library(${PROJECT_NAME} STATIC ${PUBLIC_HEADERS} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX "d")

target_include_directories(${PROJECT_NAME}
	PUBLIC
    # where the top-level project will look for the library's public headers
		"$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
	# where external projects will look for the library's public headers
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)


set_target_properties(${PROJECT_NAME} PROPERTIES 
			PUBLIC_HEADER "${PUBLIC_HEADERS}"
)

target_link_libraries(${PROJECT_NAME} PUBLIC
								${OpenCV_LIBS} 
								nlohmann_json nlohmann_json::nlohmann_json 
								spdlog::spdlog
								SPSCQueue::SPSCQueue
)

# ---------------------------------------------------------------------------------------
# Build Example App
# ---------------------------------------------------------------------------------------

if(BUILD_EXAMPLE_APP)
	message("Build example app")
	target_compile_definitions(${PROJECT_NAME} PUBLIC "INIT_AWS")
	add_subdirectory ("example")
endif()



# ---------------------------------------------------------------------------------------
# Build Tests
# ---------------------------------------------------------------------------------------
if(BUILD_TESTS)
	message("Build tests")
	enable_testing()
	add_subdirectory("tests/Fonttik.Tests")
endif()

# ---------------------------------------------------------------------------------------
# Export Target
# ---------------------------------------------------------------------------------------

if(EXPORT_FONTTIK)
	message("Export Fonttik")
	
    include(GNUInstallDirs)
set(namespace ${PROJECT_NAME})

# install the target and create export-set
install(TARGETS ${PROJECT_NAME}
    EXPORT "${PROJECT_NAME}Targets"
    # these variable get default values from GNUInstallDirs
    #RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # bin
    #LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
    #ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
    PUBLIC_HEADER DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${PROJECT_NAME}
    INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR} # include
)

# generate and install export file
install(EXPORT "${PROJECT_NAME}Targets"
    FILE "${PROJECT_NAME}Targets.cmake"
    NAMESPACE ${namespace}::
    DESTINATION cmake
)

include(CMakePackageConfigHelpers)

# generate the version file for the config file
write_basic_package_version_file(
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    VERSION "${version}"
    COMPATIBILITY AnyNewerVersion
)

# create config file
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    INSTALL_DESTINATION cmake
)

# install config files
install(FILES
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
    DESTINATION cmake
)

# generate the export targets for the build tree
export(EXPORT "${PROJECT_NAME}Targets"
    FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Targets.cmake"
    NAMESPACE ${namespace}::
)


endif()