/*
 * test_power_of_power.cpp
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

  std::println("Testing Power of Power Simplification Rules\n");
  int failures = 0;

  // Test 1: (x^2)^3 -> x^6
  {
    constexpr auto x2 = x ^ two;
    constexpr auto expr = x2 ^ three;

    // Expected: power<x, 6>
    constexpr bool is_pow = is_power_expr_v<decltype(expr)>;
    constexpr bool base_is_x = are_same_symbolic_value_v<get_lhs_t<decltype(expr)>, decltype(x)>;
    constexpr bool exp_is_6 =
      is_constant_symbol_v<get_rhs_t<decltype(expr)>> && (get_rhs_t<decltype(expr)>::value == 6);

    std::println("Test 1: (x^2)^3 -> x^6");
    if (is_pow && base_is_x && exp_is_6)
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

  // Test 2: (x^2)^0 -> 1
  // Handled by x^0 rule or (x^n)^0 -> x^0 -> 1
  {
    constexpr auto x2 = x ^ two;
    constexpr auto expr = x2 ^ constant_symbol<0>{};

    constexpr bool is_one = is_constant_symbol_v<decltype(expr)> && (decltype(expr)::value == 1);

    std::println("\nTest 2: (x^2)^0 -> 1");
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
