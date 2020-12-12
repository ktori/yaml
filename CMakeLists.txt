cmake_minimum_required(VERSION 3.16)
project(yaml C)

set(CMAKE_C_STANDARD 90)

add_library(yaml)

target_compile_options(yaml PRIVATE -Wall -Werror -ansi -pedantic -D_POSIX_C_SOURCE=200112L)

target_include_directories(yaml PUBLIC include)
target_sources(yaml PUBLIC
        include/yaml/yaml.h
        include/yaml/callbacks.h
        include/yaml/document/document.h
        include/yaml/document/kv.h
        include/yaml/document/map.h
        include/yaml/document/value.h
        include/yaml/document/sequence.h
)

add_subdirectory(src)
add_subdirectory(test)