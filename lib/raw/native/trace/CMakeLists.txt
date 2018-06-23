cmake_minimum_required(VERSION 3.8)
project(trace)

set(CMAKE_CXX_STANDARD 11)

set(SOURCE_FILES ATraceRoute.h jni.h jni_md.h traceroute.c traceroute.h ATraceToute.cpp)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../../../../out)
add_library(trace SHARED ${SOURCE_FILES})