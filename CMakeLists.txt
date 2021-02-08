cmake_minimum_required(VERSION 3.17)
project(FirstTask)

set(CMAKE_CXX_STANDARD 14)

add_executable(FirstTask
        main.cpp
        Parser.cpp
        Parser.h Node/Node.h Node/Lambda.cpp Node/Lambda.h Node/Application.cpp Node/Application.h Node/Variable.cpp Node/Variable.h)