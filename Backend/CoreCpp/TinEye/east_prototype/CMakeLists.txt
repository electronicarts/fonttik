#Recursively adds all .h and .cpp in subfolders
file(GLOB_RECURSE SOURCES ./*.h ./*.cpp)

#If needed, you can exclude files from the target like this
#list(REMOVE_ITEM SOURCES Excludethis.cpp)

add_executable(EAST ${SOURCES})

target_link_libraries(EAST ${OpenCV_LIBS})

#Copy data to destination folder
add_custom_command(TARGET EAST POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/TinEye/data $<TARGET_FILE_DIR:EAST>)