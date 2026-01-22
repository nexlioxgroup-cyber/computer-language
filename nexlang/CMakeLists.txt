cmake_minimum_required(VERSION 3.10)
project(nexlang)

set(CMAKE_CXX_STANDARD 17)

add_executable(nexlang main.cpp)

# Modular subdirectories
add_subdirectory(lexer)
add_subdirectory(parser)
add_subdirectory(runtime)
add_subdirectory(compiler)
add_subdirectory(engine)
add_subdirectory(analyzer)
add_subdirectory(symbol)
add_subdirectory(builtins)
add_subdirectory(driver)
add_subdirectory(tests)
