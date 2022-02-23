#Recursively adds all .h and .cpp in subfolders
file(GLOB_RECURSE SOURCES ./*.h ./*.cpp)

#If needed, you can exclude files from the target like this
#list(REMOVE_ITEM SOURCES Excludethis.cpp)

#Dependency include directories
#include_directories()

#Internal include directories
include_directories("../TinEyeLib/include/"  #TODO make this absolute from a TINEYE_CORE_PATH variable
"../TinEyeLib/include_private/") #TODO change includes so that private is no longer needed 
include_directories("include/" "include_private/")

add_executable(TineEye_CLI ${SOURCES})

target_link_libraries(TineEye_CLI PRIVATE
TinEyeLib
${TinEye_DEPS})

#Copy data to destination folder
add_custom_command(TARGET TineEye_CLI POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/TinEye/data $<TARGET_FILE_DIR:TineEye_CLI>)