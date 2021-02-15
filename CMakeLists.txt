cmake_minimum_required(VERSION 3.15.3)
project(FirstTask)

set(CMAKE_CXX_STANDARD 14)

add_executable(FirstTask
        SecondTask.cpp
        Parser/Parser.cpp
        Parser/Parser.h
        Node/Node.h
        Node/Lambda.cpp
        Node/Lambda.h
        Node/Application.cpp
        Node/Application.h
        Node/Variable.cpp
        Node/Variable.h
        Unification/Deducer.cpp
        Unification/Deducer.h
        Unification/Type.h
        Unification/Equation.h )