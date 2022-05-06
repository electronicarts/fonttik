file(GLOB_RECURSE SOURCES ./*.h ./*.cpp)

set(BENCHMARK_PROJECT TinEyeLib.Benchmarks)
find_package(benchmark CONFIG REQUIRED)

add_executable(${BENCHMARK_PROJECT} ${SOURCES} )

target_link_libraries(${BENCHMARK_PROJECT} PRIVATE TinEyeLib benchmark::benchmark_main)

target_include_directories(${BENCHMARK_PROJECT} PRIVATE "../TinEyeLib/include" "../TinEyeLib/include_private")

#Copy data to destination folder
add_custom_command(TARGET ${BENCHMARK_PROJECT} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/TinEye/data $<TARGET_FILE_DIR:TinEyeLib.Benchmarks>)