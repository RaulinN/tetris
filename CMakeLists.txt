cmake_minimum_required(VERSION 3.27)
project(tetris C)

set(CMAKE_C_STANDARD 99)

if (CMAKE_BUILD_TYPE MATCHES Debug)
    add_definitions(-DDEBUG=1)
endif()

add_executable(tetris src/main.c
        src/log.h)
