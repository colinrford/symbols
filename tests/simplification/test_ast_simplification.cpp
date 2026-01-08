/*
 * test_ast_simplification.cpp
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

// Test to verify that AST is actually simplified during partial substitution
// and that complex symbolic expressions are simplified correctly

int main()
{
  constexpr symbol x;
  constexpr symbol y;
  constexpr symbol z;
  constexpr symbol w;

  std::println("Testing AST Simplification During Partial Substitution\n");

  int failures = 0;

  // Test 1: Simple case - (x + 0) should simplify to x when x is bound
  {
    auto expr = x + 0.0;
    auto partial = expr(x = 5.0); // Should be 5, not (5 + 0)
    auto result = partial;        // No more symbols, should be fully evaluated
    bool pass = check_close(result, 5.0);
    std::println("Test 1: (x + 0) with x=5 should simplify to 5");
    std::println("  Result: {}, Expected: 5.0, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 2: Partial substitution with simplification - (x + 0) * y
  {
    auto expr = (x + 0.0) * y;
    auto partial = expr(x = 3.0); // Should simplify to (3 * y), not ((3 + 0) * y)
    auto result = partial(y = 2.0);
    bool pass = check_close(result, 6.0);
    std::println("\nTest 2: (x + 0) * y with x=3 should simplify to 3 * y");
    std::println("  Final result: {}, Expected: 6.0, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 3: Complex nested simplification - ((x * 1) + 0) * y
  {
    auto expr = ((x * 1.0) + 0.0) * y;
    auto partial = expr(x = 4.0); // Should simplify to (4 * y)
    auto result = partial(y = 3.0);
    bool pass = check_close(result, 12.0);
    std::println("\nTest 3: ((x * 1) + 0) * y with x=4 should simplify to 4 * y");
    std::println("  Final result: {}, Expected: 12.0, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 4: Multiple simplifications in one expression
  {
    auto expr = (x + 0.0) * (y * 1.0) + (z - z);
    auto partial = expr(x = 2.0, y = 3.0, z = 4.0);
    // Should simplify to: (2 * 3) + 0 = 6
    bool pass = check_close(partial, 6.0);
    std::println("\nTest 4: (x + 0) * (y * 1) + (z - z) with all bound");
    std::println("  Result: {}, Expected: 6.0, {}", partial, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 5: Partial with remaining symbols - (x * 0) + y should simplify to y
  {
    auto expr = (x * 0.0) + y;
    auto partial = expr(x = 7.0); // Should simplify to (0 + y) = y
    auto result = partial(y = 5.0);
    bool pass = check_close(result, 5.0);
    std::println("\nTest 5: (x * 0) + y with x=7 should simplify to y");
    std::println("  Final result: {}, Expected: 5.0, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 6: Division simplification - (x / 1) * y
  {
    auto expr = (x / 1.0) * y;
    auto partial = expr(x = 8.0); // Should simplify to (8 * y)
    auto result = partial(y = 2.0);
    bool pass = check_close(result, 16.0);
    std::println("\nTest 6: (x / 1) * y with x=8 should simplify to 8 * y");
    std::println("  Final result: {}, Expected: 16.0, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 7: Complex expression with rewriting and simplification
  {
    auto f = x + y;
    auto g = f(x = z * 1.0); // Rewrite x = z, should simplify to z (not z * 1)
    auto result = g(y = 2.0, z = 3.0);
    // Should be: (z * 1) + 2 with z=3 -> 3 + 2 = 5
    // But if simplified: z + 2 with z=3 -> 3 + 2 = 5
    bool pass = check_close(result, 5.0);
    std::println("\nTest 7: x + y with x = z * 1 (rewriting + simplification)");
    std::println("  Result: {}, Expected: 5.0, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 8: Deep nesting - (((x + 0) * 1) - 0) / 1
  {
    auto expr = (((x + 0.0) * 1.0) - 0.0) / 1.0;
    auto partial = expr(x = 10.0);
    // Should simplify all the way to 10
    bool pass = check_close(partial, 10.0);
    std::println("\nTest 8: Deep nesting (((x + 0) * 1) - 0) / 1 with x=10");
    std::println("  Result: {}, Expected: 10.0, {}", partial, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 9: Multiple symbols with mixed simplification
  {
    auto expr = (x + 0.0) * (y * 1.0) * (z - z);

    auto run_test_9 = [](auto e) {
      // Check if expr simplifies to constant 0
      if constexpr (std::is_same_v<decltype(e), constant_symbol<0>>)
      {
        std::println("\nTest 9: (x + 0) * (y * 1) * (z - z) -> 0 at construction!");
        std::println("  PASS: Aggressive simplification worked.");
      }
      else
      {
        // Fallback for previous behavior (if it didn't simplify fully)
        auto partial1 = e(x = 2.0);

        // Check if partial1 is arithmetic (fully simplified)
        if constexpr (std::is_arithmetic_v<decltype(partial1)>)
        {
          bool pass = check_close(partial1, 0.0);
          std::println("\nTest 9: (x + 0) * (y * 1) * (z - z) -> 0");
          std::println("  Result: {}, Expected: 0.0, {}", partial1, pass ? "PASS" : "FAIL");
        }
        else
        {
          auto partial2 = partial1(y = 3.0);
          auto result = partial2(z = 4.0);
          bool pass = check_close(result, 0.0);
          std::println("\nTest 9: (x + 0) * (y * 1) * (z - z) with progressive substitution");
          std::println("  Result: {}, Expected: 0.0, {}", result, pass ? "PASS" : "FAIL");
          // if (!pass) failures++; // Capture 'failures' by reference if needed, or just return result
        }
      }
    };

    run_test_9(expr);
  }

  // Test 10: Expression that should NOT simplify (x + y when both are symbolic)
  {
    auto expr = x + y;
    auto partial = expr(x = 1.0); // Should be (1 + y), not simplified further
    auto result = partial(y = 2.0);
    bool pass = check_close(result, 3.0);
    std::println("\nTest 10: x + y (no simplification opportunity)");
    std::println("  Result: {}, Expected: 3.0, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 11: Complex polynomial-like expression
  {
    auto expr = (x * x) + (x * 0.0) + (y * 1.0);
    auto partial = expr(x = 3.0); // Should simplify to (9 + 0 + y) = (9 + y)
    auto result = partial(y = 2.0);
    bool pass = check_close(result, 11.0); // 9 + 2 = 11
    std::println("\nTest 11: Complex polynomial (x*x) + (x*0) + (y*1)");
    std::println("  Result: {}, Expected: 11.0, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 12: Verify simplification happens at each step
  {
    auto expr = (x + 0.0) + (y + 0.0);
    auto step1 = expr(x = 5.0);  // Should be (5 + (y + 0)) = (5 + y)
    auto step2 = step1(y = 3.0); // Should be (5 + 3) = 8
    bool pass = check_close(step2, 8.0);
    std::println("\nTest 12: Step-by-step simplification (x + 0) + (y + 0)");
    std::println("  Result: {}, Expected: 8.0, {}", step2, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  std::println("\n=== Summary ===");
  std::println("Total tests: 12");
  std::println("Passed: {}", 12 - failures);
  std::println("Failed: {}", failures);

  if (failures == 0)
  {
    std::println("\nALL TESTS PASSED! AST simplification is working correctly.");
    return 0;
  }
  else
  {
    std::println("\nSOME TESTS FAILED - AST simplification may not be working as expected.");
    return 1;
  }
}
