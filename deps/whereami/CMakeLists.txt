cmake_minimum_required(VERSION 3.11)
project(whereami)

add_library(whereami STATIC src/whereami.h src/whereami.c)
target_include_directories(whereami PUBLIC "${CMAKE_CURRENT_LIST_DIR}/src")