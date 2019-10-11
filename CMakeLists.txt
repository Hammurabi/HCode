cmake_minimum_required(VERSION 3.13)
project(HCode)

set(CMAKE_CXX_STANDARD 14)

add_executable(HCode
        Source/Tokens.cpp Source/Tokens.h
        Source/Lexer.cpp Source/Lexer.h
        Source/Parser.cpp Source/Parser.h
        Source/Compiler.cpp Source/Compiler.h
        Source/VM.cpp Source/VM.h Source/Tests/MainTests.cpp)