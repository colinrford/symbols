/*
 * test_pattern_matching.cpp
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

// Tests for pattern matching simplification rules

int main()
{
  std::println("Testing Pattern Matching Simplification\n");

  constexpr symbol a;
  constexpr symbol b;
  constexpr symbol x;
  constexpr symbol y;

  int failures = 0;
  int tests = 0;

  auto check = [&](bool pass, const char* name) {
    tests++;
    std::println("{}: {}", name, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  };

  // Test 1: x + x → 2 * x
  {
    auto expr = x + x;
    auto result = expr(x = 5.0);
    check(check_close(result, 10.0), "x + x → 2*x (evaluates to 10 when x=5)");
  }

  // Test 2: Verify x + x is simplified at construction (not x + x)
  {
    auto expr = x + x;
    // Check if type is constant_symbol<2> * x, not x + x
    constexpr bool is_mul = is_mul_expr_v<decltype(expr)>;
    check(is_mul, "x + x type is multiplication (2*x), not addition");
  }

  // Test 3: (a + b) - b → a
  {
    auto expr = (a + b) - b;
    // Should simplify to just 'a'
    auto result = expr(a = 7.0); // Only need to bind 'a' if simplified correctly
    check(check_close(result, 7.0), "(a + b) - b → a");
  }

  // Test 4: (a + b) - a → b
  {
    auto expr = (a + b) - a;
    // Should simplify to just 'b'
    auto result = expr(b = 3.0); // Only need to bind 'b' if simplified correctly
    check(check_close(result, 3.0), "(a + b) - a → b");
  }

  // Test 5: (a * b) / b → a
  {
    auto expr = (a * b) / b;
    // Should simplify to just 'a'
    auto result = expr(a = 9.0); // Only need to bind 'a' if simplified correctly
    check(check_close(result, 9.0), "(a * b) / b → a");
  }

  // Test 6: (a * b) / a → b
  {
    auto expr = (a * b) / a;
    // Should simplify to just 'b'
    auto result = expr(b = 4.0); // Only need to bind 'b' if simplified correctly
    check(check_close(result, 4.0), "(a * b) / a → b");
  }

  // Test 7: x^0 → 1
  {
    auto expr = x ^ constant_symbol<0>{};
    constexpr bool is_one = std::is_same_v<decltype(expr), constant_symbol<1>>;
    check(is_one, "x^0 → constant_symbol<1>");
  }

  // Test 8: x^1 → x (evaluates correctly)
  {
    auto expr = x ^ constant_symbol<1>{};
    // Should evaluate to just x
    auto result = expr(x = 7.0);
    check(check_close(result, 7.0), "x^1 → x (evaluates to 7 when x=7)");
  }

  // Test 9: 0^x → 0
  {
    auto expr = constant_symbol<0>{} ^ x;
    constexpr bool is_zero = std::is_same_v<decltype(expr), constant_symbol<0>>;
    check(is_zero, "0^x → constant_symbol<0>");
  }

  // Test 10: 1^x → 1
  {
    auto expr = constant_symbol<1>{} ^ x;
    constexpr bool is_one = std::is_same_v<decltype(expr), constant_symbol<1>>;
    check(is_one, "1^x → constant_symbol<1>");
  }

  // Test 11: Complex expression ((a+b) - b) * 2 → a * 2
  {
    auto expr = ((a + b) - b) * 2.0;
    auto result = expr(a = 5.0);
    check(check_close(result, 10.0), "((a+b) - b) * 2 → a * 2");
  }

  // Test 12: Chained: (a*b) / b + (a*b) / a → a + b
  {
    auto expr = (a * b) / b + (a * b) / a;
    auto result = expr(a = 3.0, b = 4.0);
    check(check_close(result, 7.0), "(a*b)/b + (a*b)/a → a + b");
  }

  std::println("\n=== Summary ===");
  std::println("Total tests: {}", tests);
  std::println("Passed: {}", tests - failures);
  std::println("Failed: {}", failures);

  if (failures == 0)
  {
    std::println("\nALL TESTS PASSED! Pattern matching is working correctly.");
    return 0;
  }
  else
  {
    std::println("\nSOME TESTS FAILED.");
    return 1;
  }
}
