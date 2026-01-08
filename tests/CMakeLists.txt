# Test utilities module
add_library(test_utils)
target_sources(test_utils PUBLIC
  FILE_SET CXX_MODULES FILES test_utils.cppm
)
target_compile_features(test_utils PUBLIC cxx_std_23)

function(create_test name source_file)
    add_executable(${name} ${source_file})
    target_link_libraries(${name} symbols test_utils)
    target_compile_features(${name} PUBLIC cxx_std_23)
    target_include_directories(${name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    add_test(NAME ${name} COMMAND ${name})
endfunction()

include_directories(${PROJECT_SOURCE_DIR}/src)

# === Core ===
create_test(test_symbol core/test_symbol.cpp)
create_test(test_symbol_ids core/test_symbol_ids.cpp)
create_test(test_safety core/test_safety.cpp)
create_test(test_partial core/test_partial.cpp)
create_test(test_ast_checks core/test_ast_checks.cpp)

# === Simplification ===
create_test(test_simplification simplification/test_simplification.cpp)
create_test(test_ast_simplification simplification/test_ast_simplification.cpp)
create_test(test_division_simplification simplification/test_division_simplification.cpp)
create_test(test_flattening simplification/test_flattening.cpp)
create_test(test_power_simplification simplification/test_power_simplification.cpp)
create_test(test_power_of_power simplification/test_power_of_power.cpp)
create_test(test_unary_simplification simplification/test_unary_simplification.cpp)
create_test(test_subexpr_simplify simplification/test_subexpr_simplify.cpp)
create_test(test_symbolic_cancellation simplification/test_symbolic_cancellation.cpp)
create_test(test_simplification_limits simplification/test_simplification_limits.cpp)
create_test(test_complex_symbolic_simplification simplification/test_complex_symbolic_simplification.cpp)

# === Patterns ===
create_test(test_pattern_matching patterns/test_pattern_matching.cpp)
create_test(test_algebraic_patterns patterns/test_algebraic_patterns.cpp)
create_test(test_advanced_pattern patterns/test_advanced_pattern.cpp)
create_test(test_rewriting patterns/test_rewriting.cpp)

# === Integration ===
create_test(test_comprehensive integration/test_comprehensive.cpp)
create_test(test_custom_function integration/test_custom_function.cpp)
create_test(test_coverage_boundary integration/test_coverage_boundary.cpp)

add_subdirectory(assembly)
