
# Dependencies
find_package(benchmark CONFIG REQUIRED)

add_executable(Fonttik.Benchmarks opencv_mt_benchmarks.cpp)

target_link_libraries(Fonttik.Benchmarks PRIVATE fonttik benchmark::benchmark_main)

#Copy data to destination folder
add_custom_command(TARGET Fonttik.Benchmarks POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory 
		${CMAKE_CURRENT_SOURCE_DIR}/config 
		$<TARGET_FILE_DIR:Fonttik.Benchmarks>/config
	COMMENT "Copying config to benchmarks output dir"
)

add_custom_command(TARGET Fonttik.Benchmarks POST_BUILD 
	COMMAND ${CMAKE_COMMAND} -E copy_directory 
		${CMAKE_CURRENT_SOURCE_DIR}/media 
		$<TARGET_FILE_DIR:Fonttik.Benchmarks>/media
	COMMENT "Copying media to benchmarks output dir"
)