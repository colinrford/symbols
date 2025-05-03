include_directories(${PROJECT_SOURCE_DIR}/src)
add_executable(test_symbol test_symbol.cpp)

target_include_directories(test_symbol PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_sources(test_symbol PUBLIC
        FILE_SET CXX_MODULES
        TYPE CXX_MODULES
        BASE_DIRS ${PROJECT_SOURCE_DIR}/src
        FILES ${PROJECT_SOURCE_DIR}/src/symbol.cppm)
target_link_libraries(test_symbol symbol)
target_compile_features(test_symbol PUBLIC cxx_std_23)
