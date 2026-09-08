cmake_minimum_required(VERSION 4.0.2)
project(coreNumeric)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(coreNumeric
        core_numeric.h
        test.cpp
)