cmake_minimum_required(VERSION 3.16)
project(MEB-TOI)

set(CMAKE_CXX_STANDARD 14)

add_executable(TOi main.cpp)

add_executable(CFG_OPDR
        main.cpp
        CFG.cpp CFG.h
        Objects.cpp Objects.h
        )

add_executable(CFG_OPDR2
        main.cpp
        CFG.h CFG.cpp
        Objects.h Objects.cpp
        json.hpp
        )

