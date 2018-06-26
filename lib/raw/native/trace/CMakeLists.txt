cmake_minimum_required(VERSION 3.8)
project(trace)

set(CMAKE_C_STANDARD 99)

set(SOURCE_FILES ATraceRoute.h ATraceToute.c traceroute.c traceroute.h jni.h jni_md.h traceroute.c)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../../out)
add_library(trace SHARED ${SOURCE_FILES})