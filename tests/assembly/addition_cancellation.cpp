/*
 * addition_cancellation.cpp
 * part of test suite for lam.symbols
 * Assembly comparison: f(x) = x vs g(x) = (x + y) - y
 * Both should produce identical assembly when simplified.
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info 
 */

import std;
import lam.symbols;
using namespace lam::symbols;

__attribute__((noinline)) double identity_expr(double val)
{
  constexpr symbol x;
  constexpr auto f = x; // f(x) = x
  return f(x = val);
}

__attribute__((noinline)) double cancel_expr(double val)
{
  constexpr symbol x;
  constexpr symbol y;
  constexpr auto g = (x + y) - y; // g(x,y) = (x + y) - y -> should simplify to x
  return g(x = val);              // y is unbound, but should be eliminated
}

int main()
{
  volatile double input = 7.0;

  double result1 = identity_expr(input);
  double result2 = cancel_expr(input);

  std::println("identity_expr(7.0) = {}", result1);
  std::println("cancel_expr(7.0) = {}", result2);
  std::println("Match: {}", (result1 == result2) ? "YES" : "NO");

  return (result1 == result2) ? 0 : 1;
}
