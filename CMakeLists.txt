
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

project(lam_symbols VERSION 0.2.20260118 LANGUAGES CXX)

# Emscripten needs manual __CMAKE::CXX23 target creation since CMake's
# detection doesn't find the std modules in Emscripten's toolchain
if(EMSCRIPTEN AND NOT TARGET "__CMAKE::CXX23")
  set(_emscripten_libcxx_path "/usr/local/Cellar/emscripten/4.0.23/libexec/llvm/share/libc++/v1")
  if(EXISTS "${_emscripten_libcxx_path}/std.cppm")
    add_library(__cmake_cxx23 STATIC)
    target_sources(__cmake_cxx23 INTERFACE "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,STATIC_LIBRARY>:$<TARGET_OBJECTS:__cmake_cxx23>>")
    set_property(TARGET __cmake_cxx23 PROPERTY EXCLUDE_FROM_ALL 1)
    set_property(TARGET __cmake_cxx23 PROPERTY CXX_SCAN_FOR_MODULES 1)
    set_property(TARGET __cmake_cxx23 PROPERTY CXX_MODULE_STD 0)
    target_compile_features(__cmake_cxx23 PUBLIC cxx_std_23)
    target_compile_options(__cmake_cxx23 PRIVATE -Wno-reserved-module-identifier)
    target_include_directories(__cmake_cxx23 PRIVATE
      "${_emscripten_libcxx_path}"
      "/usr/local/Cellar/emscripten/4.0.23/libexec/cache/sysroot/include/c++/v1"
      "/usr/local/Cellar/emscripten/4.0.23/libexec/cache/sysroot/include")
    target_sources(__cmake_cxx23
      PUBLIC
      FILE_SET std TYPE CXX_MODULES
        BASE_DIRS "${_emscripten_libcxx_path}"
        FILES "${_emscripten_libcxx_path}/std.cppm" "${_emscripten_libcxx_path}/std.compat.cppm")
    add_library(__CMAKE::CXX23 ALIAS __cmake_cxx23)
    list(APPEND CMAKE_CXX_COMPILER_IMPORT_STD "23")
    message(STATUS "Manually created __CMAKE::CXX23 target for Emscripten")
  else()
    message(WARNING "Emscripten std.cppm not found at ${_emscripten_libcxx_path}")
  endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(Git)
if(GIT_FOUND)
  execute_process(
    COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    OUTPUT_VARIABLE GIT_HASH
    OUTPUT_STRIP_TRAILING_WHITESPACE
  )
else()
  set(GIT_HASH "unknown")
endif()

if(CMAKE_BUILD_TYPE AND CMAKE_BUILD_TYPE MATCHES "^[Dd][Ee][Bb][Uu][Gg]$")
  set(IS_DEBUG "true")
else()
  set(IS_DEBUG "false")
endif()

configure_file(
  symbols_config.cppm.in
  ${CMAKE_CURRENT_BINARY_DIR}/symbols_config.cppm
)

add_library(symbols src/symbols.cppm)
add_library(lam_symbols::symbols ALIAS symbols)
add_library(lam::symbols ALIAS symbols)
target_sources(symbols PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src ${CMAKE_CURRENT_BINARY_DIR}
        FILES 
            src/symbols.cppm
            src/symbols-traits.cppm
            src/symbols-core.cppm
            src/symbols-engine.cppm
            src/symbols-operators.cppm
            ${CMAKE_CURRENT_BINARY_DIR}/symbols_config.cppm
)
target_compile_features(symbols PUBLIC cxx_std_23)
set_target_properties(symbols PROPERTIES OUTPUT_NAME lam_symbols)

enable_testing()
add_subdirectory(tests)
add_subdirectory(examples)

# === INSTALLATION ===
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

# On macOS with -DCMAKE_INSTALL_PREFIX=/usr/local:
#   Libraries     -> /usr/local/lib/liblam_symbols.a
#   CMake config  -> /usr/local/lib/cmake/lam_symbols/
#   C++ modules   -> /usr/local/lib/c++/v1/

install(TARGETS symbols
  EXPORT lam_symbolsTargets
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  FILE_SET CXX_MODULES DESTINATION ${CMAKE_INSTALL_LIBDIR}/c++/v1
)

install(EXPORT lam_symbolsTargets
  FILE lam_symbolsTargets.cmake
  NAMESPACE lam::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lam_symbols
)

file(WRITE "${CMAKE_CURRENT_BINARY_DIR}/lam_symbolsConfig.cmake" [[
include(CMakeFindDependencyMacro)
include("${CMAKE_CURRENT_LIST_DIR}/lam_symbolsTargets.cmake")
# Backward compatibility alias
if(TARGET lam_symbols::symbols AND NOT TARGET lam::symbols)
  add_library(lam::symbols ALIAS lam_symbols::symbols)
endif()
]])

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/lam_symbolsConfigVersion.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/lam_symbolsConfig.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/lam_symbolsConfigVersion.cmake"
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/lam_symbols
)
