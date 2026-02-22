cmake_minimum_required(VERSION 3.16)


list(APPEND EXTRA_COMPONENT_DIRS "../..")                       #Path to the main project
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/components")   #Path to the esp-idf

set(COMPONENTS main 01_basic_test)

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(build_test)