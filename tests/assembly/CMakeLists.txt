# Assembly comparison tests
# These are built WITHOUT optimization flags to see raw codegen

function(create_assembly_test name)
    add_executable(asm_${name} ${name}.cpp)
    target_link_libraries(asm_${name} symbols)
    target_compile_features(asm_${name} PUBLIC cxx_std_23)
    target_include_directories(asm_${name} PUBLIC ${CMAKE_CURRENT_BINARY_DIR})
    # No optimization flags - we want to see unoptimized codegen
    target_compile_options(asm_${name} PRIVATE -O0 -g)
    add_test(NAME asm_${name} COMMAND asm_${name})
endfunction()

create_assembly_test(identity_vs_simplified)
create_assembly_test(addition_cancellation)
create_assembly_test(multiplication_identity)
create_assembly_test(zero_addition)
create_assembly_test(power_simplification)
create_assembly_test(power_ext_assembly)
create_assembly_test(mixed_operations)
create_assembly_test(basic_ops_assembly)
create_assembly_test(complex_ops_assembly)
create_assembly_test(combined_assembly)
create_test(asm_edge_cases_assembly edge_cases_assembly.cpp)
create_test(asm_subtraction_cancellation subtraction_cancellation.cpp)

# Specialized check for optimization capabilities
# explicit -O3 to verify zero-cost abstractions
add_library(assembly_check_legacy OBJECT assembly_check.cpp)
target_link_libraries(assembly_check_legacy PUBLIC symbols)
target_compile_features(assembly_check_legacy PUBLIC cxx_std_23)
target_compile_options(assembly_check_legacy PRIVATE -O3)
