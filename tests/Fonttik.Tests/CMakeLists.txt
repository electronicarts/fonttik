
# Source files
set(SOURCE_FILES 
	configuration_tests.cpp
	contrast_tests.cpp
	Fonttik_tests.cpp
	image_tests.cpp
	luminance_tests.cpp
	results_tests.cpp
	size_tests.cpp
	textbox_merging_tests.cpp
	video_tests.cpp
)

set(TEST_PROJECT Fonttik.Tests)

# Dependencies
find_package(GTest CONFIG REQUIRED)

# Target definitons
add_executable(${TEST_PROJECT} ${SOURCE_FILES} )

if(UNIX)
	target_compile_options(${TEST_PROJECT} PUBLIC -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(${TEST_PROJECT} PUBLIC -fsanitize=address)
endif()

target_link_libraries(${TEST_PROJECT} PUBLIC fonttik GTest::gtest_main)
target_include_directories(${TEST_PROJECT} PRIVATE "${FONTTIK_PRIVATE_HEADERS}")

include(GoogleTest)

gtest_discover_tests(${TEST_PROJECT})

#Copy data to destination folder
add_custom_command(TARGET ${TEST_PROJECT} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:Fonttik.Tests>)