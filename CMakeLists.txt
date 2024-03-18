cmake_minimum_required(VERSION 3.22)

project(interpreter LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(interpreter src/main.cpp src/Tokenizer.hpp src/Parser.hpp)
