file(GLOB_RECURSE SOURCES ./*.h ./*.cpp)

set(TEST_PROJECT TinEyeLib.Tests)
find_package(GTest CONFIG REQUIRED)

add_executable(${TEST_PROJECT} ${SOURCES} )

target_link_libraries(${TEST_PROJECT} PRIVATE TinEyeLib GTest::gtest_main)

target_include_directories(${TEST_PROJECT} PRIVATE "../TinEyeLib/include" "../TinEyeLib/include_private")

include(GoogleTest)

gtest_discover_tests(${TEST_PROJECT})

#Copy data to destination folder
add_custom_command(TARGET ${TEST_PROJECT} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/TinEye/data $<TARGET_FILE_DIR:TinEyeLib.Tests>)