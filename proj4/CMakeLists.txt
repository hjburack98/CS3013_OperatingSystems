cmake_minimum_required(VERSION 3.14)
project(proj4 C)

set(CMAKE_C_STANDARD 11)

include_directories(.)

add_executable(proj4
        proj4.c
        proj4.h)
