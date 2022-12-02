#Recursively adds all .h and .cpp in subfolders
file(GLOB_RECURSE SOURCES ./*.h ./*.cpp)

#If needed, you can exclude files from the target like this
#list(REMOVE_ITEM SOURCES Excludethis.cpp)

#Dependency include directories
#include_directories()

#Internal include directories
include_directories("../FonttikLib/include/")  #Make this a variable
include_directories("include/" "include_private/")

add_executable(FonttikApp_MT ${SOURCES})

target_link_libraries(FonttikApp_MT PRIVATE
FonttikLib
${Fonttik_DEPS})

#Copy data to destination folder
add_custom_command(TARGET FonttikApp_MT POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Fonttik/data $<TARGET_FILE_DIR:FonttikApp_MT>)