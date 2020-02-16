cmake_minimum_required(VERSION 3.16)
project(yaml C)

set(CMAKE_C_STANDARD 90)

add_library(yaml)

target_compile_options(yaml PRIVATE -Wall -Werror -ansi -pedantic -D_POSIX_C_SOURCE=200112L)

add_subdirectory(src)
add_subdirectory(test)