cmake_minimum_required(VERSION 3.0)
project(LdH_CPP_STD)

set(CMAKE_CXX_STANDARD 20)
set(LdH_CPP_STD_DEFINED)
add_library(LdH_CPP_STD INTERFACE)
target_include_directories(LdH_CPP_STD INTERFACE ./include)
