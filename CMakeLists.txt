cmake_minimum_required(VERSION 3.22)
project(dns_server)

add_subdirectory(UT)

file(GLOB src 
    "*.c"
    "*.h"
)

add_executable(proxy ${src})