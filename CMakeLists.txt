cmake_minimum_required(VERSION 3.0)
project(LdH_CppStd)

set(CMAKE_CXX_STANDARD 20)

add_library(LdH_CppStd INTERFACE)
target_include_directories(LdH_CppStd INTERFACE ./include)


if (0)
    add_executable(test main.cpp)
    target_link_libraries(test PRIVATE LdH_CppStd)
endif ()