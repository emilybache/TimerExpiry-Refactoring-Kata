cmake_minimum_required(VERSION 3.13)
project(alarm_clock_refactoring_kata)

set(CMAKE_CXX_STANDARD 11)

# uncomment this line to enable coverage measurements using gcov
set(CMAKE_CXX_FLAGS "--coverage")

enable_testing()
add_subdirectory(tests)

