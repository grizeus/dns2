cmake_minimum_required(VERSION 3.22)
project(dns_server)

set(CMAKE_BUILD_TYPE Debug)
add_subdirectory(UT)

file(GLOB src
    "*.c"
    "*.h"
)

add_executable(proxy ${src})
