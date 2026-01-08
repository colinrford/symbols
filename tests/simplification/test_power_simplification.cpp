/*
 * test_power_simplification.cpp
 * part of test suite for lam.symbols
 * Extending Author: Colin Ford
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info
 */

import std;
import lam.symbols;
using namespace lam::symbols;

template<typename T>
void print_type()
{
  std::println("Type: {}", typeid(T).name());
}

int main()
{
  constexpr symbol x;
  constexpr constant_symbol<2> two;

  std::println("Testing Power Simplification Rules\n");
  int failures = 0;

  // Test 1: x^2 + x^2 -> 2 * x^2
  // Requires simplify_add to recognize x^2 as a term with coefficient 1 and base x^2,
  // and then add coefficients.
  {
    constexpr auto x2 = x ^ two;
    auto expr = x2 + x2;

    // Expected structure: constant_symbol<2> * (x^2)
    // Check if it's a multiplication expression
    constexpr bool is_mul = is_mul_expr_v<decltype(expr)>;
    // Check LHS is constant 2
    constexpr bool lhs_is_2 =
      is_constant_symbol_v<get_lhs_t<decltype(expr)>> && (get_lhs_t<decltype(expr)>::value == 2);
    // Check RHS is x^2
    constexpr bool rhs_is_x2 = are_same_symbolic_value_v<get_rhs_t<decltype(expr)>, decltype(x2)>;

    std::println("Test 1: x^2 + x^2 -> 2 * x^2");
    if (is_mul && lhs_is_2 && rhs_is_x2)
    {
      std::println("  PASS: Simplified to 2 * x^2");
    }
    else
    {
      std::println("  FAIL: Did not simplify as expected");
      print_type<decltype(expr)>();
      failures++;
    }
  }

  // Test 2: x^2 * x^2 -> x^4
  // Requires simplify_mul to recognize base matches and add exponents.
  {
    constexpr auto x2 = x ^ two;
    auto expr = x2 * x2;

    // Expected structure: x^4 (power expression, base x, exp 4)
    constexpr bool is_pow = is_power_expr_v<decltype(expr)>;
    constexpr bool base_is_x = are_same_symbolic_value_v<get_lhs_t<decltype(expr)>, decltype(x)>;
    constexpr bool exp_is_4 =
      is_constant_symbol_v<get_rhs_t<decltype(expr)>> && (get_rhs_t<decltype(expr)>::value == 4);

    std::println("\nTest 2: x^2 * x^2 -> x^4");
    if (is_pow && base_is_x && exp_is_4)
    {
      std::println("  PASS: Simplified to x^4");
    }
    else
    {
      std::println("  FAIL: Did not simplify to x^4");
      print_type<decltype(expr)>();
      // We expect this to fail currently based on code analysis
      failures++;
    }
  }

  return failures == 0 ? 0 : 1;
}
