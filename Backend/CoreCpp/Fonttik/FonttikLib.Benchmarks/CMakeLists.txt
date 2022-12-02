file(GLOB_RECURSE SOURCES ./*.h ./*.cpp)

set(BENCHMARK_PROJECT FonttikLib.Benchmarks)
find_package(benchmark CONFIG REQUIRED)

add_executable(${BENCHMARK_PROJECT} ${SOURCES} )

target_link_libraries(${BENCHMARK_PROJECT} PRIVATE FonttikLib benchmark::benchmark_main)

target_include_directories(${BENCHMARK_PROJECT} PRIVATE "../FonttikLib/include" "../FonttikLib/include_private")

#Copy data to destination folder
add_custom_command(TARGET ${BENCHMARK_PROJECT} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/Fonttik/data $<TARGET_FILE_DIR:FonttikLib.Benchmarks>)