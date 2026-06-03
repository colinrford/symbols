
cmake_minimum_required(VERSION 3.31.6 FATAL_ERROR)

# C++23 import-std UUID ladder + standard settings (must precede project()).
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/LamCxxModuleStd.cmake)

# VERSION file = single source of truth (CMake + Conan read the same string).
file(READ "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" _lam_symbols_version)
string(STRIP "${_lam_symbols_version}" _lam_symbols_version)
project(lam_symbols VERSION ${_lam_symbols_version} LANGUAGES CXX)

# Re-run CMake configure whenever VERSION changes.
set_property(DIRECTORY APPEND PROPERTY
  CMAKE_CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")

# Emscripten needs manual __CMAKE::CXX23 target creation since CMake's
# detection doesn't find the std modules in Emscripten's toolchain
if(EMSCRIPTEN AND NOT TARGET "__CMAKE::CXX23")
  set(_emscripten_std_modules "${CMAKE_CURRENT_SOURCE_DIR}/../cmake/emscripten-std-modules")
  set(_emscripten_libcxx_headers "/usr/local/Cellar/emscripten/5.0.0/libexec/cache/sysroot/include/c++/v1")
  set(_emscripten_sysroot "/usr/local/Cellar/emscripten/5.0.0/libexec/cache/sysroot/include")
  if(EXISTS "${_emscripten_std_modules}/std.cppm")
    add_library(__cmake_cxx23 STATIC)
    target_sources(__cmake_cxx23 INTERFACE "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,STATIC_LIBRARY>:$<TARGET_OBJECTS:__cmake_cxx23>>")
    set_property(TARGET __cmake_cxx23 PROPERTY EXCLUDE_FROM_ALL 1)
    set_property(TARGET __cmake_cxx23 PROPERTY CXX_SCAN_FOR_MODULES 1)
    set_property(TARGET __cmake_cxx23 PROPERTY CXX_MODULE_STD 0)
    target_compile_features(__cmake_cxx23 PUBLIC cxx_std_23)
    target_compile_options(__cmake_cxx23 PRIVATE -Wno-reserved-module-identifier)
    target_include_directories(__cmake_cxx23 PRIVATE
      "${_emscripten_libcxx_headers}"
      "${_emscripten_sysroot}")
    target_sources(__cmake_cxx23
      PUBLIC
      FILE_SET std TYPE CXX_MODULES
        BASE_DIRS "${_emscripten_std_modules}"
        FILES "${_emscripten_std_modules}/std.cppm" "${_emscripten_std_modules}/std.compat.cppm")
    add_library(__CMAKE::CXX23 ALIAS __cmake_cxx23)
    list(APPEND CMAKE_CXX_COMPILER_IMPORT_STD "23")
    message(STATUS "Manually created __CMAKE::CXX23 target for Emscripten")
  else()
    message(WARNING "Emscripten std.cppm not found at ${_emscripten_std_modules}")
  endif()
endif()

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Vendored lam build helpers (lam_add_config_partition, lam_find_dependency,
# lam_add_config_dump, lam_install_module_package).
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/LamModule.cmake)

# Experimental partitions (laws / lawsets / morphisms / calculus.diff) are held
# off the default public surface until they stabilize. When OFF (default) they
# are not compiled, not installed, and not re-exported from lam.symbols; the
# matching #ifdef in src/symbols.cppm and the gated tests follow this flag.
option(LAM_SYMBOLS_EXPERIMENTAL "Build + export the experimental symbols partitions" OFF)

add_library(symbols src/symbols.cppm)
add_library(lam_symbols::symbols ALIAS symbols)
add_library(lam::symbols ALIAS symbols)
target_sources(symbols PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
        FILES
            src/symbols.cppm
            src/symbols-traits.cppm
            src/symbols-core.cppm
            src/symbols-engine.cppm
            src/symbols-operators.cppm
)

if(LAM_SYMBOLS_EXPERIMENTAL)
  target_sources(symbols PUBLIC
          FILE_SET CXX_MODULES
          TYPE CXX_MODULES
          BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/src
          FILES
              src/symbols-laws.cppm
              src/symbols-lawsets.cppm
              src/symbols-morphisms.cppm
              src/symbols-calculus.diff.cppm
  )
  target_compile_definitions(symbols PUBLIC LAM_SYMBOLS_EXPERIMENTAL)
endif()

target_compile_features(symbols PUBLIC cxx_std_23)

# Consumer dependency: find_package(lam_concepts) + BMI-synthesis property dance.
lam_find_dependency(lam_concepts)
target_link_libraries(symbols PUBLIC lam_concepts::concepts)

set_target_properties(symbols PROPERTIES OUTPUT_NAME lam_symbols)

# Generate + attach the lam.symbols:config partition (build-time git_hash refresh).
lam_add_config_partition(symbols NAME symbols)

enable_testing()

# config_dump smoke test + build-config diagnostic (imports lam.symbols).
lam_add_config_dump(symbols NAME symbols)

add_subdirectory(tests)
add_subdirectory(examples)

# On macOS with -DCMAKE_INSTALL_PREFIX=/usr/local:
#   Libraries     -> /usr/local/lib/liblam_symbols.a
#   CMake config  -> /usr/local/lib/cmake/lam_symbols/
#   C++ modules   -> /usr/local/include/lam/symbols/
#   CPS metadata  -> /usr/local/lib/cps/
lam_install_module_package(symbols
  NAME        symbols
  DESCRIPTION "lam.symbols — symbolic calculus engine (C++23 modules)"
  LICENSE     "CC0-1.0"
  REQUIRES    lam_concepts
)
