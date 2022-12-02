set(TEST_PROJECT AddressSanitizer.Tests)

find_package(GTest CONFIG REQUIRED)

set(Header_Files
    "include/AddressSanitizerTest.h"    
)
source_group("Header Files" FILES ${Header_Files})

set(Source_Files
   "src/AddressSanitizerTest.cpp"
)

source_group("Source Files" FILES ${Source_Files})

set(ALL_FILES
    ${Header_Files}
    ${Source_Files}
    )

add_executable(${TEST_PROJECT} ${ALL_FILES})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")

target_link_libraries(${TEST_PROJECT} PUBLIC GTest::gtest_main PRIVATE -fsanitize=address)

# add the binary tree to the search path for include files
# so that we will find library headers
target_include_directories(${TEST_PROJECT} PRIVATE
                          "${PROJECT_SOURCE_DIR}/Fonttik/AddressSanitizer.Tests/include"
                           )


include(GoogleTest)
gtest_discover_tests(${TEST_PROJECT})