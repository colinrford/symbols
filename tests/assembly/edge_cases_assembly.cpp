/*
 * edge_cases_assembly.cpp
 * part of test suite for lam.symbols
 * Assembly comparison for edge cases: x/x, 0/x, x^0, cancellation
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info */

import std;
import lam.symbols;
using namespace lam::symbols;

// --- Division by self ---
__attribute__((noinline)) double check_div_self(double val)
{
  constexpr symbol x;
  constexpr auto f = x / x;
  return f(x = val);
}

__attribute__((noinline)) double baseline_one(double val)
{
  constexpr auto f = constant_symbol<1>{};
  return f();
}

// --- Division of zero ---
__attribute__((noinline)) double check_div_zero_numerator(double val)
{
  constexpr symbol x;
  constexpr auto f = constant_symbol<0>{} / x;
  return f(x = val);
}

__attribute__((noinline)) double baseline_zero(double val)
{
  constexpr auto f = constant_symbol<0>{};
  return f();
}

// --- Power zero ---
__attribute__((noinline)) double check_pow_zero(double val)
{
  constexpr symbol x;
  constexpr auto f = x ^ constant_symbol<0>{};
  return f(x = val);
}

// --- Nested Cancellation ---
// x - (x + 1) -> -1
__attribute__((noinline)) double check_nested_cancellation(double val)
{
  constexpr symbol x;
  constexpr auto f = x - (x + constant_symbol<1>{});
  return f(x = val);
}

__attribute__((noinline)) double baseline_minus_one(double val)
{
  constexpr auto f = constant_symbol<-1>{};
  return f();
}

int main()
{
  volatile double v = 10.0;

  // x / x -> 1
  if (check_div_self(v) != baseline_one(v))
    return 1;

  // 0 / x -> 0 (assuming x != 0)
  if (check_div_zero_numerator(v) != baseline_zero(v))
    return 2;

  // x ^ 0 -> 1
  if (check_pow_zero(v) != baseline_one(v))
    return 3;

  // x - (x + 1) -> -1
  if (check_nested_cancellation(v) != baseline_minus_one(v))
    return 4;

  std::println("Edge cases runtime checks passed.");
  return 0;
}
