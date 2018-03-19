cmake_minimum_required(VERSION 3.9)
project(ass3 C)

set(CMAKE_C_STANDARD 99)

add_executable(ass3 main.c simulator.h list.h simulator.c list.c)