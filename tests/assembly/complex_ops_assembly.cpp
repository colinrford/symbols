/*
 * complex_ops_assembly.cpp
 * part of test suite for lam.symbols
 * Assembly comparison for complex individual operations
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info */

import std;
import lam.symbols;
using namespace lam::symbols;

// --- Nested Additions ---
__attribute__((noinline)) double check_nested_add(double val)
{
  constexpr symbol x;
  constexpr auto f = (x + constant_symbol<1>{}) + constant_symbol<2>{};
  return f(x = val);
}

__attribute__((noinline)) double baseline_add_three(double val)
{
  constexpr symbol x;
  constexpr auto f = x + constant_symbol<3>{};
  return f(x = val);
}

// --- Term Collection ---
__attribute__((noinline)) double check_term_collection(double val)
{
  constexpr symbol x;
  constexpr auto f = x + x;
  return f(x = val);
}

__attribute__((noinline)) double baseline_mul_two(double val)
{
  constexpr symbol x;
  constexpr auto f = constant_symbol<2>{} * x; // 2*x
  return f(x = val);
}

// --- Power of Power ---
__attribute__((noinline)) double check_power_of_power(double val)
{
  constexpr symbol x;
  constexpr constant_symbol<2> two;
  constexpr constant_symbol<3> three;
  constexpr auto f = (x ^ two) ^ three;
  return f(x = val);
}

__attribute__((noinline)) double baseline_pow_six(double val)
{
  constexpr symbol x;
  constexpr auto f = x ^ constant_symbol<6>{};
  return f(x = val);
}


int main()
{
  volatile double v = 2.0;

  if (std::abs(check_nested_add(v) - baseline_add_three(v)) > 1e-9)
    return 1;
  if (std::abs(check_term_collection(v) - baseline_mul_two(v)) > 1e-9)
    return 2;
  // Power check might be loose on assembly but strict on value
  if (std::abs(check_power_of_power(v) - baseline_pow_six(v)) > 1e-9)
    return 3;

  std::println("Complex operations runtime checks passed.");
  return 0;
}
