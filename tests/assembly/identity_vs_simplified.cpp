/*
 * identity_vs_simplified.cpp
 * part of test suite for lam.symbols
 * Assembly comparison: f(x) = x vs g(x) = x^2/x
 * Both should produce identical assembly when simplified.
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info */

import std;
import lam.symbols;
using namespace lam::symbols;

// Prevent inlining to see clear function boundaries
__attribute__((noinline)) double identity_expr(double val)
{
  constexpr symbol x;
  constexpr auto f = x; // f(x) = x
  return f(x = val);
}

__attribute__((noinline)) double simplified_expr(double val)
{
  constexpr symbol x;
  constexpr auto g = (x ^ constant_symbol<2>{}) / x; // g(x) = x^2 / x -> should simplify to x
  return g(x = val);
}

int main()
{
  volatile double input = 5.0; // volatile to prevent constant propagation

  double result1 = identity_expr(input);
  double result2 = simplified_expr(input);

  std::println("identity_expr(5.0) = {}", result1);
  std::println("simplified_expr(5.0) = {}", result2);
  std::println("Match: {}", (result1 == result2) ? "YES" : "NO");

  return (result1 == result2) ? 0 : 1;
}
