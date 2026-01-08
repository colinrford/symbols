/*
 * test_partial.cpp
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

// Helper to print type name (for debugging if needed, though we rely on runtime values)
// void print_type(auto) { std::println("{}", __PRETTY_FUNCTION__); }

int main()
{
  constexpr symbol a;
  constexpr symbol b;
  constexpr symbol c;

  // 1. Construct a simple expression: x = a + b
  constexpr auto x = a + b;

  // 2. Partial substitution: replace a with 5.0
  // expected: result is (5.0 + b) which is a symbolic expression
  auto x_partial = x(a = 5.0);

  std::println("Partial substitution done.");

  // 3. Evaluate the rest: replace b with 2.0
  // expected: (5.0 + 2.0) -> 7.0
  double result = x_partial(b = 2.0);

  std::println("Result: {}", result); // Should be 7.0

  if (!check_close(result, 7.0))
  {
    std::println("FAIL: Expected 7.0, got {}", result);
    return 1;
  }

  // 4. Test mixed arithmetic in chain
  // f = a * b * c
  constexpr auto f = a * b * c;

  // partial: bind a=2
  auto f1 = f(a = 2.0); // -> 2.0 * b * c

  // partial: bind b=3
  auto f2 = f1(b = 3.0); // -> 2.0 * 3.0 * c -> 6.0 * c (if evaluated left-to-right?)
                         // Actually our operator+ returns symbolic_expression<plus, double, symbol>
                         // So it holds the logic.

  // full: bind c=4
  double val = f2(c = 4.0);
  std::println("Chained result: {}", val); // 24.0

  if (!check_close(val, 24.0))
  {
    std::println("FAIL: Expected 24.0, got {}", val);
    return 1;
  }

  std::println("SUCCESS");
  return 0;
}
