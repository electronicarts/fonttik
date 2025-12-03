project(Fonttik.Tests)

# Source files
set(SOURCE_FILES 
	configuration_tests.cpp
	contrast_tests.cpp
	Fonttik_tests.cpp
	image_tests.cpp
#	luminance_tests.cpp #--Temporary disabled luminance tests
	size_tests.cpp
	textbox_merging_tests.cpp
	video_tests.cpp
	colorblindness_tests.cpp
)

# Dependencies
find_package(GTest CONFIG REQUIRED)

# Target definitons
add_executable(${PROJECT_NAME} ${SOURCE_FILES} )

if(UNIX)
	target_compile_options(${PROJECT_NAME} PUBLIC -fsanitize=address -fno-omit-frame-pointer)
    target_link_options(${PROJECT_NAME} PUBLIC -fsanitize=address)
endif()

target_link_libraries(${PROJECT_NAME} PUBLIC fonttik GTest::gtest_main)
target_include_directories(${PROJECT_NAME} PRIVATE "${FONTTIK_PRIVATE_HEADERS}")

include(GoogleTest)

gtest_discover_tests(${PROJECT_NAME})

#Copy data to destination folder
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data $<TARGET_FILE_DIR:Fonttik.Tests>)

if(BUILD_COVERAGE)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/data ${CMAKE_BINARY_DIR})
	include(Coverage)
    AddCoverage(${PROJECT_NAME})
endif()