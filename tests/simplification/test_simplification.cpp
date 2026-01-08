/*
 * test_simplification.cpp
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

int main()
{
  constexpr symbol x;
  constexpr symbol y;
  constexpr symbol z;
  constexpr constant_symbol<0> zero;
  constexpr constant_symbol<1> one;

  std::println("Testing Simplification Rules\n");

  int failures = 0;

  // Test 1: x + 0 -> x
  {
    auto expr = x + 0.0;
    auto result = expr(x = 5.0);
    bool pass = check_close(result, 5.0);
    std::println("Test 1: x + 0 -> x");
    std::println("  Expression: x + 0, with x=5, result: {}", result);
    std::println("  Expected: 5.0, Got: {}, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 2: 0 + x -> x
  {
    auto expr = 0.0 + x;
    auto result = expr(x = 3.0);
    bool pass = check_close(result, 3.0);
    std::println("\nTest 2: 0 + x -> x");
    std::println("  Expression: 0 + x, with x=3, result: {}", result);
    std::println("  Expected: 3.0, Got: {}, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 3: x - 0 -> x
  {
    auto expr = x - 0.0;
    auto result = expr(x = 7.0);
    bool pass = check_close(result, 7.0);
    std::println("\nTest 3: x - 0 -> x");
    std::println("  Expression: x - 0, with x=7, result: {}", result);
    std::println("  Expected: 7.0, Got: {}, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 4: x - x -> 0
  {
    auto expr = x - x;
    auto result = expr(x = 10.0);
    bool pass = check_close(result, 0.0);
    std::println("\nTest 4: x - x -> 0");
    std::println("  Expression: x - x, with x=10, result: {}", result);
    std::println("  Expected: 0.0, Got: {}, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 5: x * 1 -> x
  {
    auto expr = x * 1.0;
    auto result = expr(x = 4.0);
    bool pass = check_close(result, 4.0);
    std::println("\nTest 5: x * 1 -> x");
    std::println("  Expression: x * 1, with x=4, result: {}", result);
    std::println("  Expected: 4.0, Got: {}, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 6: 1 * x -> x
  {
    auto expr = 1.0 * x;
    auto result = expr(x = 6.0);
    bool pass = check_close(result, 6.0);
    std::println("\nTest 6: 1 * x -> x");
    std::println("  Expression: 1 * x, with x=6, result: {}", result);
    std::println("  Expected: 6.0, Got: {}, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 7: x * 0 -> 0
  {
    auto expr = x * 0.0;
    auto result = expr(x = 100.0);
    bool pass = check_close(result, 0.0);
    std::println("\nTest 7: x * 0 -> 0");
    std::println("  Expression: x * 0, with x=100, result: {}", result);
    std::println("  Expected: 0.0, Got: {}, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 8: 0 * x -> 0
  {
    auto expr = 0.0 * x;
    auto result = expr(x = 50.0);
    bool pass = check_close(result, 0.0);
    std::println("\nTest 8: 0 * x -> 0");
    std::println("  Expression: 0 * x, with x=50, result: {}", result);
    std::println("  Expected: 0.0, Got: {}, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 9: x / 1 -> x
  {
    auto expr = x / 1.0;
    auto result = expr(x = 8.0);
    bool pass = check_close(result, 8.0);
    std::println("\nTest 9: x / 1 -> x");
    std::println("  Expression: x / 1, with x=8, result: {}", result);
    std::println("  Expected: 8.0, Got: {}, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 10: x / x -> 1 (when x != 0)
  {
    auto expr = x / x;
    auto result = expr(x = 9.0);
    bool pass = check_close(result, 1.0);
    std::println("\nTest 10: x / x -> 1");
    std::println("  Expression: x / x, with x=9, result: {}", result);
    std::println("  Expected: 1.0, Got: {}, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 11: Simplification with partial substitution
  {
    auto expr = x + 0.0;
    auto partial = expr(x = 2.0); // Should simplify to 2.0 + 0 -> 2.0
    // partial should still be symbolic if there are other symbols, but here it's just 2.0
    // Actually, if x is bound, we get 2.0 + 0.0 which should simplify to 2.0
    bool pass = check_close(partial, 2.0);
    std::println("\nTest 11: Simplification with partial substitution");
    std::println("  Expression: x + 0, with x=2, result: {}", partial);
    std::println("  Expected: 2.0, Got: {}, {}", partial, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 12: Nested simplification: (x + 0) * 1 -> x
  {
    auto expr = (x + 0.0) * 1.0;
    auto result = expr(x = 5.0);
    bool pass = check_close(result, 5.0);
    std::println("\nTest 12: Nested simplification (x + 0) * 1 -> x");
    std::println("  Expression: (x + 0) * 1, with x=5, result: {}", result);
    std::println("  Expected: 5.0, Got: {}, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  // Test 13: Complex expression with multiple simplifications
  {
    auto expr = (x + 0.0) * (y * 1.0) + (z - z);
    auto result = expr(x = 2.0, y = 3.0, z = 4.0);
    bool pass = check_close(result, 6.0); // (2 + 0) * (3 * 1) + (4 - 4) = 2 * 3 + 0 = 6
    std::println("\nTest 13: Complex simplification");
    std::println("  Expression: (x + 0) * (y * 1) + (z - z), with x=2, y=3, z=4, result: {}", result);
    std::println("  Expected: 6.0, Got: {}, {}", result, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  std::println("\n=== Summary ===");
  std::println("Total tests: 13");
  std::println("Passed: {}", 13 - failures);
  std::println("Failed: {}", failures);

  if (failures == 0)
  {
    std::println("\nALL TESTS PASSED!");
    return 0;
  }
  else
  {
    std::println("\nSOME TESTS FAILED");
    return 1;
  }
}
