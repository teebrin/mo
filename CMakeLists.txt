cmake_minimum_required(VERSION 4.2)
project(mo)

set(CMAKE_CXX_STANDARD 23)

enable_testing()

add_subdirectory(sources)
add_subdirectory(tests)