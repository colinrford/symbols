/*
 * test_unary_simplification.cpp
 * part of test suite for lam.symbols
 * Extending Author: Colin Ford
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info
 */

import std;
import lam.symbols;

using namespace lam::symbols;

int main()
{
  constexpr symbol x;

  std::println("Testing Unary Simplification rules\n");
  int failures = 0;

  // Test 1: Double Negation -> Identity
  // -(-x) should simplify to x
  {
    auto expr = -(-x);
    using Expect = std::remove_cvref_t<decltype(x)>;
    using Actual = std::remove_cvref_t<decltype(expr)>;

    bool pass = std::is_same_v<Actual, Expect>;
    std::println("Test 1: -(-x) -> x");
    std::println("  Result Type: {}, Expected: x, {}", typeid(Actual).name(), pass ? "PASS" : "FAIL");

    if (!pass)
      failures++;
  }

  // Test 2: Negation of Zero -> Zero
  // -0 should simplify to 0
  {
    constexpr auto z = constant_symbol<0>{};
    auto expr = -z;
    constexpr bool is_zero = is_constant_symbol_v<decltype(expr)> && (decltype(expr)::value == 0);

    std::println("Test 2: -0 -> 0");
    std::println("  Result Is Zero: {}, {}", is_zero, is_zero ? "PASS" : "FAIL");

    if (!is_zero)
      failures++;
  }

  // Test 3: Unary Identity (fallback)
  // sin(x) should remain sin(x) as we haven't implemented specific sin rules yet
  // but the dispatcher shouldn't crash
  /*
  {
       auto expr = std::abs(x);
       // Note: std::abs might not work out of box if not overloaded for symbol
       // We'll skip this for now as we don't have std::abs wrapper handy
  }
  */

  // Test 4: Evaluation still works
  {
    auto expr = -(-x);
    auto val = expr(x = 5.0); // should be 5.0
    bool pass = std::abs(val - 5.0) < 1e-9;
    std::println("Test 4: Eval -(-x) with x=5");
    std::println("  Result: {}, Expected: 5.0, {}", val, pass ? "PASS" : "FAIL");
    if (!pass)
      failures++;
  }

  if (failures == 0)
  {
    std::println("\nALL UNARY TESTS PASSED");
    return 0;
  }
  else
  {
    std::println("\nSOME TESTS FAILED");
    return 1;
  }
}
