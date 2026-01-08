/*
 * test_symbolic_cancellation.cpp
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
  constexpr symbol y;
  constexpr symbol z;


  // Check 1: (x + y) - y
  auto expr1 = (x + y) - y;
  static_assert(std::is_same_v<std::remove_cvref_t<decltype(expr1)>, std::remove_cvref_t<decltype(x)>>,
                "(x+y)-y should simplify to x");

  // Check 2: (x * y) / y
  auto expr2 = (x * y) / y;
  static_assert(std::is_same_v<std::remove_cvref_t<decltype(expr2)>, std::remove_cvref_t<decltype(x)>>,
                "(x*y)/y should simplify to x");

  // Check 3: x - x
  auto expr3 = x - x;
  static_assert(std::is_same_v<decltype(expr3), constant_symbol<0>>, "x-x should simplify to 0");

  // Check 4: Symbolic Factorization (y*x + z*x)
  auto expr4 = (y * x) + (z * x);
  // Should ideally be (y+z)*x, but we expect it checks fail (returns sum expression)
  bool is_sum = is_plus_expr_v<decltype(expr4)>;
  if (is_sum)
  {
    std::cout << "y*x + z*x is still a sum (Not Factorized)" << std::endl;
  }
  else
  {
    std::cout << "y*x + z*x simplified!" << std::endl;
  }

  return 0;
}
