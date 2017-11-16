cmake_minimum_required(VERSION 3.8)
project(brainfuck-interpreter)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES main.c)

add_executable(brainfuck-interpreter ${SOURCE_FILES})

target_compile_options(
        brainfuck-interpreter
        PRIVATE
        -Wall -Werror -Wextra -Wpedantic -Wformat=2
        )
