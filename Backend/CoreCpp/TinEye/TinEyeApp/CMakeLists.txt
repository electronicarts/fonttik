#Recursively adds all .h and .cpp in subfolders
file(GLOB_RECURSE SOURCES ./*.h ./*.cpp)

#If needed, you can exclude files from the target like this
#list(REMOVE_ITEM SOURCES Excludethis.cpp)

#Dependency include directories
#include_directories()

#Internal include directories
include_directories("../TinEyeLib/include/")  #TODO make this absolute from a TINEYE_CORE_PATH variable
include_directories("include/" "include_private/")

add_executable(TinEyeApp ${SOURCES})

target_link_libraries(TinEyeApp PRIVATE
TinEyeLib
${TinEye_DEPS})

#Copy data to destination folder
add_custom_command(TARGET TinEyeApp POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/TinEye/data $<TARGET_FILE_DIR:TinEyeApp>)