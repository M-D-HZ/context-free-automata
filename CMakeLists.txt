cmake_minimum_required(VERSION 3.16)
project(MEB-TOI)

set(CMAKE_CXX_STANDARD 14)

add_executable(TOi main.cpp)

add_executable(CFG_OPDR
        main.cpp
        src/CFG.cpp src/CFG.h
        src/Objects.cpp src/Objects.h
        )

