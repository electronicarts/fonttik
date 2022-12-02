file(GLOB_RECURSE SOURCES ./*.h ./*.cpp)

set(TEST_PROJECT FonttikLib.Tests)
find_package(GTest CONFIG REQUIRED)

add_executable(${TEST_PROJECT} ${SOURCES} )

#set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")

target_link_libraries(${TEST_PROJECT} PUBLIC FonttikLib GTest::gtest_main) #PRIVATE -fsanitize=address)

target_include_directories(${TEST_PROJECT} PRIVATE "../FonttikLib/include" "../FonttikLib/include_private")

include(GoogleTest)

gtest_discover_tests(${TEST_PROJECT})

#Copy data to destination folder
add_custom_command(TARGET ${TEST_PROJECT} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Fonttik/data $<TARGET_FILE_DIR:FonttikLib.Tests>)