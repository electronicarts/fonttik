#Recursively adds all .h and .cpp in subfolders
file(GLOB_RECURSE SOURCES ./*.h ./*.cpp)

#If needed, you can exclude files from the target like this
#list(REMOVE_ITEM SOURCES Excludethis.cpp)

#Internal include directories
include_directories("include/" "include_private/")

if(PROFILING)
	add_definitions(-DPROFILING)
endif()


add_library(FonttikLib STATIC ${SOURCES})
target_include_directories(FonttikLib PUBLIC "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}/include")

target_link_libraries(FonttikLib PRIVATE ${Fonttik_DEPS} spdlog::spdlog)
