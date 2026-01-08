/*
 * test_algebraic_patterns.cpp
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

// Tests for additional algebraic patterns (subtraction inverses, squaring, power combining)

int main()
{
  std::println("Testing Additional Algebraic Patterns\n");

  constexpr symbol a;
  constexpr symbol b;
  constexpr symbol x;

  int failures = 0;
  int tests = 0;

  auto check = [&](bool pass, const char* name) {
    tests++;
    std::println("{}: {}", name, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  };

  // === Subtraction Inverse Patterns ===
  std::println("--- Subtraction Inverses ---");

  // Test 1: (a - b) + b → a
  {
    auto expr = (a - b) + b;
    auto result = expr(a = 7.0); // Should only need 'a' if simplified
    check(check_close(result, 7.0), "(a - b) + b → a");
  }

  // Test 2: b + (a - b) → a
  {
    auto expr = b + (a - b);
    auto result = expr(a = 9.0); // Should only need 'a' if simplified
    check(check_close(result, 9.0), "b + (a - b) → a");
  }

  // Test 3: a - (a - b) → b
  {
    auto expr = a - (a - b);
    auto result = expr(b = 5.0); // Should only need 'b' if simplified
    check(check_close(result, 5.0), "a - (a - b) → b");
  }

  // === Squaring Pattern ===
  std::println("\n--- Squaring (x * x → x²) ---");

  // Test 4: x * x → x^2
  {
    auto expr = x * x;
    // Should be a power expression
    constexpr bool is_pow = is_power_expr_v<decltype(expr)>;
    check(is_pow, "x * x type is power expression (x^2)");
  }

  // Test 5: x * x evaluates correctly
  {
    auto expr = x * x;
    auto result = expr(x = 3.0);
    check(check_close(result, 9.0), "x * x evaluates to 9 when x=3");
  }

  // === Power Combining Patterns ===
  std::println("\n--- Power Combining ---");

  // Test 6: x^2 * x → x^3
  {
    auto x_squared = x ^ constant_symbol<2>{};
    auto expr = x_squared * x;
    auto result = expr(x = 2.0);
    check(check_close(result, 8.0), "x^2 * x → x^3 (evaluates to 8 when x=2)");
  }

  // Test 7: x * x^2 → x^3
  {
    auto x_squared = x ^ constant_symbol<2>{};
    auto expr = x * x_squared;
    auto result = expr(x = 2.0);
    check(check_close(result, 8.0), "x * x^2 → x^3 (evaluates to 8 when x=2)");
  }

  // Test 8: x * x * x → x^3 (chained)
  {
    auto expr = x * x * x; // Should be (x^2) * x = x^3
    auto result = expr(x = 2.0);
    check(check_close(result, 8.0), "x * x * x → x^3 (chained)");
  }

  // Test 9: Verify x^2 * x produces x^3 type
  {
    auto x_squared = x ^ constant_symbol<2>{};
    auto expr = x_squared * x;
    // Check if it's x^3 (power expression with exponent 3)
    constexpr bool is_pow = is_power_expr_v<decltype(expr)>;
    check(is_pow, "x^2 * x type is power expression");
  }

  // === Complex Combinations ===
  std::println("\n--- Complex Combinations ---");

  // Test 10: (a - b) + b + (a - b) + b → a + a → 2*a
  {
    auto expr = ((a - b) + b) + ((a - b) + b);
    auto result = expr(a = 5.0);
    check(check_close(result, 10.0), "Nested subtraction inverses → 2*a");
  }

  // Test 11: x * x * x * x → x^4
  {
    auto expr = x * x * x * x;
    auto result = expr(x = 2.0);
    check(check_close(result, 16.0), "x * x * x * x → x^4 (evaluates to 16)");
  }

  std::println("\n=== Summary ===");
  std::println("Total tests: {}", tests);
  std::println("Passed: {}", tests - failures);
  std::println("Failed: {}", failures);

  if (failures == 0)
  {
    std::println("\nALL TESTS PASSED!");
    return 0;
  }
  else
  {
    std::println("\nSOME TESTS FAILED.");
    return 1;
  }
}
