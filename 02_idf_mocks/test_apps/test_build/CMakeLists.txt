cmake_minimum_required(VERSION 3.16)


list(APPEND EXTRA_COMPONENT_DIRS "../..")                           #Path to the component being tested
list(APPEND EXTRA_COMPONENT_DIRS "$ENV{IDF_PATH}/components")       #Path to the esp-idf

set(COMPONENTS main 02_idf_mocks)                                   #List of components

include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(build_test)