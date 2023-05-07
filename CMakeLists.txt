cmake_minimum_required(VERSION 3.25)
project(memcli)

set(CMAKE_CXX_STANDARD 17)

add_executable(memcli memcli.cpp memlib/Memory.h memlib/transformations.h)
