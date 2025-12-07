include_directories(${PROJECT_SOURCE_DIR}/src)
add_executable(test_symbol test_symbol.cpp)

target_include_directories(test_symbol PUBLIC ${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(test_symbol symbol)
target_compile_features(test_symbol PUBLIC cxx_std_23)

add_executable(test_partial test_partial.cpp)
target_include_directories(test_partial PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
target_link_libraries(test_partial symbol)
target_compile_features(test_partial PUBLIC cxx_std_23)
