file(GLOB_RECURSE SOURCES ./*.h ./*.cpp)

set(BENCHMARK_PROJECT TinEyeLib.Benchmarks)
find_package(benchmark REQUIRED)

add_executable(${BENCHMARK_PROJECT} ${SOURCES} )

target_link_libraries(${BENCHMARK_PROJECT} PUBLIC TinEyeLib benchmark::benchmark)

target_include_directories(${BENCHMARK_PROJECT} PRIVATE "../TinEyeLib/include" "../TinEyeLib/include_private")