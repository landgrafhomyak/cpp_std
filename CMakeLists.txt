cmake_minimum_required(VERSION 3.0)
project(LdH_CppStd)

set(CMAKE_CXX_STANDARD 20)

add_library(LdH_CppStd INTERFACE)
target_include_directories(LdH_CppStd INTERFACE ./include)
