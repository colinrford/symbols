/*
 * combined_assembly.cpp
 * part of test suite for lam.symbols
 * Assembly comparison for combined expressions
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info */

import std;
import lam.symbols;
using namespace lam::symbols;

// --- Polynomial like simplification ---
// (x+1)^2 should hopefully simplify or at least be equivalent to x^2 + 2x + 1
// But actually we want to test if large expressions simplify down.
// Let's test: (x+1)*(x-1) -> x^2 - 1

__attribute__((noinline)) double check_difference_of_squares(double val)
{
  constexpr symbol x;
  constexpr auto f = (x + constant_symbol<1>{}) * (x - constant_symbol<1>{});
  return f(x = val);
}

__attribute__((noinline)) double baseline_difference_of_squares(double val)
{
  constexpr symbol x;
  // x^2 - 1
  constexpr auto f = (x ^ constant_symbol<2>{}) - constant_symbol<1>{};
  return f(x = val);
}

// --- Rational simplification ---
// (x^2 + x) / x -> x + 1
__attribute__((noinline)) double check_rational_simp(double val)
{
  constexpr symbol x;
  constexpr auto f = ((x ^ constant_symbol<2>{}) + x) / x;
  return f(x = val);
}

__attribute__((noinline)) double baseline_plus_one(double val)
{
  constexpr symbol x;
  constexpr auto f = x + constant_symbol<1>{};
  return f(x = val);
}

int main()
{
  volatile double v = 4.0;

  // Difference of squares
  if (std::abs(check_difference_of_squares(v) - baseline_difference_of_squares(v)) > 1e-9)
    return 1;

  // Rational
  if (std::abs(check_rational_simp(v) - baseline_plus_one(v)) > 1e-9)
    return 2;

  std::println("Combined operations runtime checks passed.");
  return 0;
}
