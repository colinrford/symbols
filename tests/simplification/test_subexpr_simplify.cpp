/*
 * test_subexpr_simplify.cpp
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
  constexpr symbol a;
  constexpr symbol b;

  std::println("Testing sub-expression simplification...\n");

  // Test 1: Can (a+b) - (a+b) simplify at compile time?
  auto expr = (a + b) - (a + b);

  std::println("Type of (a+b) - (a+b): {}", typeid(expr).name());

  // Check if it's constant_symbol<0>
  constexpr bool is_zero = std::is_same_v<decltype(expr), constant_symbol<0>>;
  std::println("Is constant_symbol<0>: {}", is_zero);

  if constexpr (is_zero)
  {
    std::println("\nSUCCESS: (a+b) - (a+b) simplifies to 0 at COMPILE TIME!");
  }
  else
  {
    std::println("\nNot simplified at construction. Evaluating...");
    auto result = expr(a = 5.0, b = 3.0);
    std::println("Result: {} (expected 0)", result);
  }

  // Test 2: What about separate variables with same expression?
  auto lhs = a + b;
  auto rhs = a + b;
  auto diff = lhs - rhs;

  constexpr bool diff_is_zero = std::is_same_v<decltype(diff), constant_symbol<0>>;
  std::println("\nlhs - rhs where lhs = a+b, rhs = a+b:");
  std::println("Is constant_symbol<0>: {}", diff_is_zero);

  return 0;
}
