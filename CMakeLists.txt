
cmake_minimum_required(VERSION 3.31.6 FATAL_ERROR)
set(CMAKE_CXX_STANDARD 23)

if(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0.3)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "d0edc3af-4c50-42ea-a356-e2862fe7a444")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 4.0.0)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "a9e1cf81-9932-4810-974b-6eccaf14e457")
elseif(CMAKE_VERSION VERSION_GREATER_EQUAL 3.31.6)
  set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "0e5b6991-d74f-4b3d-a41c-cf096e0b2508")
else()
  message(FATAL_ERROR "Version lower than 3.31.6 not supported (by me)")
endif()

# from source download: 0e5b6991-d74f-4b3d-a41c-cf096e0b2508
# works for 3.31.6
# from github.com: 1942b4fa-b2c5-4546-9385-83f254070067
# from gitlab.kitware.com: a9e1cf81-9932-4810-974b-6eccaf14e457
# works for cmake 4.0.1, should work 4.0.2
# d0edc3af-4c50-42ea-a356-e2862fe7a444 # works for post-4.0.2

set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_MODULE_STD ON)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

project(symbols LANGUAGES CXX)

add_library(symbol src/symbol.cppm)
target_sources(symbol PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        FILES src/symbol.cppm
)
target_compile_features(symbol PUBLIC cxx_std_23)
add_subdirectory(tests)
