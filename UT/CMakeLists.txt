cmake_minimum_required(VERSION 3.22)

project(ut)
set(CMAKE_BUILD_TYPE Debug)

file(GLOB src
        "../*.h"
        "../binary_string.c"
        "../communicate.c"
        "../dns_parser.c"
        "../file_parser.c"
        "../map.c"
        "../utility.c"
        "main_ut.c"
)

add_executable(ut ${src})
