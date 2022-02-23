file(GLOB_RECURSE SOURCES ./*.h ./*.cpp)

add_executable(TinEyeLib_Tests ${SOURCES} )

target_link_libraries(TinEyeLib_Tests PRIVATE TinEyeLib GTest::gtest_main)

target_include_directories(TinEyeLib_Tests PRIVATE "../TinEyeLib/include" "../TinEyeLib/include_private")

include(GoogleTest)

gtest_discover_tests(TinEyeLib_Tests)

#Copy data to destination folder
add_custom_command(TARGET TinEyeLib_Tests POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/TinEye/data $<TARGET_FILE_DIR:TinEyeLib_Tests>)