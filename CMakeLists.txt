cmake_minimum_required(VERSION 3.21)
project(Chapter01)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Chapter01
        Game.cpp
        Game.h
        Main.cpp)
