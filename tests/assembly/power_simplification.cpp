/*
 * power_simplification.cpp
 * part of test suite for lam.symbols
 * Assembly comparison: f(x) = x^2 vs g(x) = x * x
 * Both should produce identical assembly.
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info */

import std;
import lam.symbols;
using namespace lam::symbols;

__attribute__((noinline)) double power_expr(double val)
{
  constexpr symbol x;
  constexpr auto f = x ^ constant_symbol<2>{}; // x^2
  return f(x = val);
}

__attribute__((noinline)) double mul_expr(double val)
{
  constexpr symbol x;
  constexpr auto g = x * x; // x * x -> x^2
  return g(x = val);
}

int main()
{
  volatile double input = 5.0;

  double r1 = power_expr(input);
  double r2 = mul_expr(input);

  std::println("power_expr(5.0) = {} (x^2)", r1);
  std::println("mul_expr(5.0) = {} (x*x)", r2);
  std::println("Match: {}", (r1 == r2) ? "YES" : "NO");

  return (r1 == r2) ? 0 : 1;
}
