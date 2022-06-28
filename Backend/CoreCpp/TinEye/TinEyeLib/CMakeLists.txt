#Recursively adds all .h and .cpp in subfolders
file(GLOB_RECURSE SOURCES ./*.h ./*.cpp)

#If needed, you can exclude files from the target like this
#list(REMOVE_ITEM SOURCES Excludethis.cpp)

#Dependency include directories
include_directories(${Boost_INCLUDE_DIRS})

#Internal include directories
include_directories("include/" "include_private/")

if(PROFILING)
	add_definitions(-DPROFILING)
endif()

add_library(TinEyeLib STATIC ${SOURCES})

target_link_libraries(TinEyeLib PRIVATE ${TinEye_DEPS} Boost::log)
