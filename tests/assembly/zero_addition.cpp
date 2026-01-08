/*
 * zero_addition.cpp
 * part of test suite for lam.symbols
 * Assembly comparison: f(x) = x vs g(x) = x + 0 vs h(x) = 0 + x
 * All should produce identical assembly when simplified.
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info */

import std;
import lam.symbols;
using namespace lam::symbols;

__attribute__((noinline)) double identity_expr(double val)
{
  constexpr symbol x;
  constexpr auto f = x;
  return f(x = val);
}

__attribute__((noinline)) double add_zero_right(double val)
{
  constexpr symbol x;
  constexpr auto g = x + constant_symbol<0>{}; // x + 0 -> x
  return g(x = val);
}

__attribute__((noinline)) double add_zero_left(double val)
{
  constexpr symbol x;
  constexpr auto h = constant_symbol<0>{} + x; // 0 + x -> x
  return h(x = val);
}

int main()
{
  volatile double input = 42.0;

  double r1 = identity_expr(input);
  double r2 = add_zero_right(input);
  double r3 = add_zero_left(input);

  std::println("identity_expr(42.0) = {}", r1);
  std::println("add_zero_right(42.0) = {}", r2);
  std::println("add_zero_left(42.0) = {}", r3);

  bool match = (r1 == r2) && (r2 == r3);
  std::println("All match: {}", match ? "YES" : "NO");

  return match ? 0 : 1;
}
