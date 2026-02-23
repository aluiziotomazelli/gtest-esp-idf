cmake_minimum_required(VERSION 3.16)

# Append extra component directories so IDF can find our local components.
list(APPEND EXTRA_COMPONENT_DIRS 
    "../.."                               # The '01_basic_test' component being tested
    "../gtest"                            # The GTest wrapper component
    "$ENV{IDF_PATH}/tools/mocks/driver"   # Path to the esp-idf driver mock
)

# Explicitly list the components to be included in the build.
# Note: 'main' is the folder inside host_test/test_sum/
set(COMPONENTS main 02_idf_mocks)

# Standard ESP-IDF project configuration.
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
project(test_sum)