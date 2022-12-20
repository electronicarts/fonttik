set(TEST_PROJECT AddressSanitizer.Tests)

find_package(GTest CONFIG REQUIRED)

add_executable(${TEST_PROJECT} AddressSanitizerTest.h AddressSanitizerTest.cpp)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_LINKER_FLAGS "${CMAKE_LINKER_FLAGS} -fno-omit-frame-pointer -fsanitize=address")

target_link_libraries(${TEST_PROJECT} PUBLIC GTest::gtest_main PRIVATE -fsanitize=address)

include(GoogleTest)
gtest_discover_tests(${TEST_PROJECT})