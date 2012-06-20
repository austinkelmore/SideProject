cmake_minimum_required(VERSION 2.6)
project(nowide)

enable_testing()

include_directories(.)

add_library(nowide src/streambuf.cpp src/convert.cpp)
add_executable(test_streambuf test/test_streambuf.cpp)
target_link_libraries(test_streambuf nowide)
add_executable(test_nowide test/test_nowide.cpp)
target_link_libraries(test_nowide nowide)


add_test(test_nowide test_nowide)
add_test(test_streambuf test_streambuf)
