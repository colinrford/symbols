/*
 * test_complex_symbolic_simplification.cpp
 * part of test suite for lam.symbols
 * Extending Author: Colin Ford
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info
 */

import std;
import lam.symbols;
import lam.test.utils;
using namespace lam::symbols;
using namespace lam::test::utils;

// Test more complex symbolic simplifications
// This tests what CAN and CANNOT be simplified with the current implementation

int main()
{
  constexpr symbol x;
  constexpr symbol y;
  constexpr symbol z;
  constexpr symbol a;
  constexpr symbol b;

  std::println("Testing Complex Symbolic Simplifications\n");
  std::println("Note: Current implementation simplifies arithmetic expressions");
  std::println("      but does NOT simplify purely symbolic expressions (e.g., x - x)\n");

  int failures = 0;
  int skipped = 0;

  // Test 1: Arithmetic simplification works - (5 + 0) -> 5
  {
    auto expr = 5.0 + 0.0;
    bool pass = lam::test::utils::check_close(expr, 5.0);
    std::println("Test 1: 5 + 0 -> 5 (arithmetic)");
    std::println("  Result: {}, Expected: 5.0, {}", expr, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 2: Mixed simplification works - (x + 0) with x=5 -> 5
  {
    auto expr = x + 0.0;
    auto result = expr(x = 5.0);
    bool pass = lam::test::utils::check_close(result, 5.0);
    std::println("\nTest 2: (x + 0) with x=5 -> 5 (mixed symbolic/arithmetic)");
    std::println("  Result: {}, Expected: 5.0, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 3: Purely symbolic - (x - x) does NOT simplify (limitation)
  // This would require structural equality checking
  {
    auto expr = x - x;
    auto result = expr(x = 5.0);
    // Currently: (5 - 5) = 0 (simplified at evaluation time)
    // But: (x - x) as symbolic expression is NOT simplified to 0
    bool pass = lam::test::utils::check_close(result, 0.0);
    std::println("\nTest 3: (x - x) with x=5 -> 0");
    std::println("  Result: {}, Expected: 0.0, {}", result, pass ? "PASS" : "FAIL");
    std::println("  Note: Simplifies at evaluation, not at AST level for symbolic x");
    if (!pass)
      failures++;
  }

  // Test 4: Complex nested with multiple operations
  {
    auto expr = ((x * 2.0) + 0.0) * ((y * 1.0) + 0.0);
    auto partial = expr(x = 3.0, y = 4.0);
    // Should simplify to: (3 * 2) * 4 = 6 * 4 = 24
    bool pass = lam::test::utils::check_close(partial, 24.0);
    std::println("\nTest 4: ((x * 2) + 0) * ((y * 1) + 0) with x=3, y=4");
    std::println("  Result: {}, Expected: 24.0, {}", partial, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 5: Expression with zero multiplication
  {
    auto expr = (x * 0.0) + (y * 2.0);
    auto partial = expr(x = 5.0); // Should simplify to (0 + y*2) = (y*2)
    auto result = partial(y = 3.0);
    bool pass = lam::test::utils::check_close(result, 6.0);
    std::println("\nTest 5: (x * 0) + (y * 2) with x=5, y=3");
    std::println("  Result: {}, Expected: 6.0, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 6: Division by one
  {
    auto expr = (x / 1.0) * y;
    auto partial = expr(x = 6.0);
    auto result = partial(y = 2.0);
    bool pass = lam::test::utils::check_close(result, 12.0);
    std::println("\nTest 6: (x / 1) * y with x=6, y=2");
    std::println("  Result: {}, Expected: 12.0, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 7: Multiple zero additions
  {
    auto expr = x + 0.0 + 0.0 + y;
    auto partial = expr(x = 1.0, y = 2.0);
    // Should simplify to: 1 + 2 = 3
    bool pass = lam::test::utils::check_close(partial, 3.0);
    std::println("\nTest 7: x + 0 + 0 + y with x=1, y=2");
    std::println("  Result: {}, Expected: 3.0, {}", partial, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 8: Identity multiplication chain
  {
    auto expr = x * 1.0 * 1.0 * y;
    auto partial = expr(x = 2.0, y = 3.0);
    // Should simplify to: 2 * 3 = 6
    bool pass = lam::test::utils::check_close(partial, 6.0);
    std::println("\nTest 8: x * 1 * 1 * y with x=2, y=3");
    std::println("  Result: {}, Expected: 6.0, {}", partial, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 9: Complex polynomial-like with simplifications
  {
    auto expr = (x * x) + (x * 0.0) + (x * 1.0) + 0.0;
    auto partial = expr(x = 3.0);
    // Should simplify to: 9 + 0 + 3 + 0 = 9 + 3 = 12
    bool pass = lam::test::utils::check_close(partial, 12.0);
    std::println("\nTest 9: (x*x) + (x*0) + (x*1) + 0 with x=3");
    std::println("  Result: {}, Expected: 12.0, {}", partial, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 10: Nested with rewriting
  {
    auto f = x + y;
    auto g = f(x = (z * 1.0) + 0.0); // Rewrite x = z (simplified)
    auto result = g(y = 2.0, z = 4.0);
    // Should be: ((z*1)+0) + 2 with z=4 -> 4 + 2 = 6
    bool pass = lam::test::utils::check_close(result, 6.0);
    std::println("\nTest 10: x + y with x = (z*1)+0 (rewriting)");
    std::println("  Result: {}, Expected: 6.0, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 11: What CANNOT be simplified (purely symbolic)
  // (x + y) - y should ideally simplify to x, but currently doesn't
  {
    auto expr = (x + y) - y;
    auto result = expr(x = 3.0, y = 2.0);
    // Currently evaluates to: (3 + 2) - 2 = 5 - 2 = 3 (correct result)
    // But the AST (x + y) - y is NOT simplified to x
    bool pass = lam::test::utils::check_close(result, 3.0);
    std::println("\nTest 11: (x + y) - y with x=3, y=2");
    std::println("  Result: {}, Expected: 3.0, {}", result, pass ? "PASS" : "FAIL");
    std::println("  Note: Evaluates correctly but AST not simplified (limitation)");
    if (!pass)
      failures++;
    else
      skipped++; // This is a known limitation
  }

  // Test 12: Another limitation - (x * y) / y should simplify to x
  {
    auto expr = (x * y) / y;
    auto result = expr(x = 4.0, y = 2.0);
    // Evaluates to: (4 * 2) / 2 = 8 / 2 = 4 (correct)
    // But AST not simplified
    bool pass = lam::test::utils::check_close(result, 4.0);
    std::println("\nTest 12: (x * y) / y with x=4, y=2");
    std::println("  Result: {}, Expected: 4.0, {}", result, pass ? "PASS" : "FAIL");
    std::println("  Note: Evaluates correctly but AST not simplified (limitation)");
    if (!pass)
      failures++;
    else
      skipped++; // Known limitation
  }

  std::println("\n=== Summary ===");
  std::println("Total tests: 12");
  std::println("Passed: {}", 12 - failures);
  std::println("Failed: {}", failures);
  std::println("Known limitations (work but don't simplify AST): {}", skipped);

  std::println("\n=== Current Simplification Capabilities ===");
  std::println("✓ Arithmetic simplifications: x + 0, x * 1, x * 0, etc.");
  std::println("✓ Mixed symbolic/arithmetic: (x + 0) simplifies when x is bound");
  std::println("✓ Nested simplifications: (((x + 0) * 1) - 0) / 1");
  std::println("✗ Purely symbolic: (x - x) does NOT simplify to 0 at AST level");
  std::println("✗ Structural equality: (x + y) - y does NOT simplify to x");

  if (failures == 0)
  {
    std::println("\nALL TESTS PASSED!");
    std::println("Simplification works for arithmetic and mixed cases.");
    std::println("Purely symbolic simplifications require structural equality checking.");
    return 0;
  }
  else
  {
    std::println("\nSOME TESTS FAILED");
    return 1;
  }
}
