cmake_minimum_required(VERSION 3.5)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

project(single_headers LANGUAGES CXX)

add_executable(single_headers main.cpp)
target_include_directories(single_headers PRIVATE include)
