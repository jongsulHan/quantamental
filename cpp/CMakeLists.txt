#"/home/hanege/miniconda3/envs/quant_sys/include/python3.11",
#"/home/hanege/miniconda3/envs/quant_sys/lib/python3.11/site-packages/pybind11/include",
cmake_minimum_required(VERSION 3.15)
project(quantamental LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Find pybind11
find_package(pybind11 REQUIRED)

# BloomFilter library sources
set(BLOOM_FILTER_SOURCES
    src/bloom_filter.cpp
    src/murmur_hash3.cpp
)

# Static library for internal use and testing
add_library(bloom_filter_lib STATIC ${BLOOM_FILTER_SOURCES})
target_include_directories(bloom_filter_lib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# Python module
pybind11_add_module(quantamental src/bindings.cpp)
target_link_libraries(quantamental PRIVATE bloom_filter_lib)

# Install the Python module to the source tree for development
set(PYTHON_MODULE_INSTALL_DIR "${CMAKE_CURRENT_SOURCE_DIR}/..")
install(TARGETS quantamental DESTINATION ${PYTHON_MODULE_INSTALL_DIR})

# Testing (optional)
option(BUILD_TESTS "Build tests" OFF)
if(BUILD_TESTS)
    enable_testing()
    find_package(GTest REQUIRED)

    add_executable(test_bloom_filter tests/test_bloom_filter.cpp)
    target_link_libraries(test_bloom_filter PRIVATE bloom_filter_lib GTest::gtest_main)

    include(GoogleTest)
    gtest_discover_tests(test_bloom_filter)
endif()
