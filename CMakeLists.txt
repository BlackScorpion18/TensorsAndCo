cmake_minimum_required(VERSION 4.2)
project(TensorsAndCo)

set(CMAKE_CXX_STANDARD 20)

add_library(TensorsAndCo STATIC library.cpp
        Tensor.cpp
        Tensor.h
)
