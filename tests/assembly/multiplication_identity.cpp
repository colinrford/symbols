/*
 * multiplication_identity.cpp
 * part of test suite for lam.symbols
 * Assembly comparison: f(x) = x vs g(x) = x * 1 vs h(x) = 1 * x
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

__attribute__((noinline)) double mul_one_right(double val)
{
  constexpr symbol x;
  constexpr auto g = x * constant_symbol<1>{}; // x * 1 -> x
  return g(x = val);
}

__attribute__((noinline)) double mul_one_left(double val)
{
  constexpr symbol x;
  constexpr auto h = constant_symbol<1>{} * x; // 1 * x -> x
  return h(x = val);
}

int main()
{
  volatile double input = 3.14;

  double r1 = identity_expr(input);
  double r2 = mul_one_right(input);
  double r3 = mul_one_left(input);

  std::println("identity_expr(3.14) = {}", r1);
  std::println("mul_one_right(3.14) = {}", r2);
  std::println("mul_one_left(3.14) = {}", r3);

  bool match = (r1 == r2) && (r2 == r3);
  std::println("All match: {}", match ? "YES" : "NO");

  return match ? 0 : 1;
}
