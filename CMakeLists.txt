
cmake_minimum_required(VERSION 3.31.6 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
# from source download: 0e5b6991-d74f-4b3d-a41c-cf096e0b2508        # works for 3.31.6
# from github.com: 1942b4fa-b2c5-4546-9385-83f254070067
# from gitlab.kitware.com: a9e1cf81-9932-4810-974b-6eccaf14e457     # works for cmake 4.0.1

set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

project(symbols LANGUAGES CXX)

add_library(symbol src/symbol.cppm)
target_sources(symbol PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES src/symbol.cppm
)
target_compile_features(symbol PUBLIC cxx_std_23)
add_subdirectory(src)
add_subdirectory(tests)
