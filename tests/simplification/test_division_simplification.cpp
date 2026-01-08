/*
 * test_division_simplification.cpp
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
  constexpr constant_symbol<3> three;

  std::println("Testing Division Simplification Rules\n");
  int failures = 0;

  // Test 1: x^2 / x -> x^1 -> x
  // Previously verified via assembly, but good to have explicit test
  {
    constexpr auto num = x ^ two;
    constexpr auto expr = num / x;

    using Expect = std::remove_cvref_t<decltype(x)>;
    using Actual = std::remove_cvref_t<decltype(expr)>;

    std::println("Test 1: x^2 / x -> x");
    if constexpr (std::is_same_v<Actual, Expect>)
    {
      std::println("  PASS");
    }
    else
    {
      std::println("  FAIL");
      print_type<decltype(expr)>();
      failures++;
    }
  }

  // Test 2: x / x^2 -> x^(-1)
  {
    constexpr auto den = x ^ two;
    constexpr auto expr = x / den;

    // Expected: power<x, -1>
    constexpr bool is_pow = is_power_expr_v<decltype(expr)>;
    constexpr bool base_is_x = are_same_symbolic_value_v<get_lhs_t<decltype(expr)>, decltype(x)>;
    constexpr bool exp_is_minus_1 =
      is_constant_symbol_v<get_rhs_t<decltype(expr)>> && (get_rhs_t<decltype(expr)>::value == -1);

    std::println("\nTest 2: x / x^2 -> x^-1");
    if (is_pow && base_is_x && exp_is_minus_1)
    {
      std::println("  PASS");
    }
    else
    {
      std::println("  FAIL");
      print_type<decltype(expr)>();
      failures++;
    }
  }

  // Test 3: x^5 / x^2 -> x^3
  {
    constexpr auto num = x ^ constant_symbol<5>{};
    constexpr auto den = x ^ two;
    constexpr auto expr = num / den;

    // Expected: power<x, 3>
    constexpr bool is_pow = is_power_expr_v<decltype(expr)>;
    constexpr bool base_is_x = are_same_symbolic_value_v<get_lhs_t<decltype(expr)>, decltype(x)>;
    constexpr bool exp_is_3 =
      is_constant_symbol_v<get_rhs_t<decltype(expr)>> && (get_rhs_t<decltype(expr)>::value == 3);

    std::println("\nTest 3: x^5 / x^2 -> x^3");
    if (is_pow && base_is_x && exp_is_3)
    {
      std::println("  PASS");
    }
    else
    {
      std::println("  FAIL");
      print_type<decltype(expr)>();
      failures++;
    }
  }

  // Test 4: x^2 / x^5 -> x^-3
  {
    constexpr auto num = x ^ two;
    constexpr auto den = x ^ constant_symbol<5>{};
    constexpr auto expr = num / den;

    // Expected: power<x, -3>
    constexpr bool is_pow = is_power_expr_v<decltype(expr)>;
    constexpr bool base_is_x = are_same_symbolic_value_v<get_lhs_t<decltype(expr)>, decltype(x)>;
    constexpr bool exp_is_minus_3 =
      is_constant_symbol_v<get_rhs_t<decltype(expr)>> && (get_rhs_t<decltype(expr)>::value == -3);

    std::println("\nTest 4: x^2 / x^5 -> x^-3");
    if (is_pow && base_is_x && exp_is_minus_3)
    {
      std::println("  PASS");
    }
    else
    {
      std::println("  FAIL");
      print_type<decltype(expr)>();
      failures++;
    }
  }

  // Test 5: x^2 / x^2 -> 1
  // This should be caught by are_same_symbolic_value check or x^(2-2) -> x^0 -> 1
  {
    constexpr auto x2 = x ^ two;
    constexpr auto expr = x2 / x2;

    constexpr bool is_one = is_constant_symbol_v<decltype(expr)> && (decltype(expr)::value == 1);

    std::println("\nTest 5: x^2 / x^2 -> 1");
    if (is_one)
    {
      std::println("  PASS");
    }
    else
    {
      std::println("  FAIL");
      print_type<decltype(expr)>();
      failures++;
    }
  }

  return failures == 0 ? 0 : 1;
}
