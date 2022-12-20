
# Dependencies
find_package(OpenCV CONFIG REQUIRED)

add_executable(FonttikApp Fonttik_cli.cpp)

target_link_libraries(FonttikApp PRIVATE fonttik ${OpenCV_LIBS})

#Copy data to destination folder
add_custom_command(TARGET FonttikApp POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/config $<TARGET_FILE_DIR:FonttikApp>)