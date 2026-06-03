// SPDX-License-Identifier: CC0-1.0
// SPDX-FileCopyrightText: 2025-2026 Colin Ford

/*
 * zero_multiplication.cpp
 * part of test suite for lam.symbols
 * Assembly comparison: f(x) = 0 vs g(x) = 0 * x vs h(x) = x * 0
 * All should produce identical assembly when simplified.
 * Tests the multiplicative annihilator: is_annihilator_v<multiplies, constant_symbol<0>>
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info */

import std;
import lam.symbols;
using namespace lam::symbols;

__attribute__((noinline)) double baseline_zero(double val)
{
  (void)val;
  return 0.0;
}

__attribute__((noinline)) double mul_zero_left(double val)
{
  constexpr symbol x;
  constexpr auto f = constant_symbol<0>{} * x; // 0 * x -> 0
  return f(x = val);
}

__attribute__((noinline)) double mul_zero_right(double val)
{
  constexpr symbol x;
  constexpr auto g = x * constant_symbol<0>{}; // x * 0 -> 0
  return g(x = val);
}

int main()
{
  volatile double input = 42.0;

  double r0 = baseline_zero(input);
  double r1 = mul_zero_left(input);
  double r2 = mul_zero_right(input);

  std::println("baseline_zero(42.0) = {}", r0);
  std::println("mul_zero_left(42.0) = {}", r1);
  std::println("mul_zero_right(42.0) = {}", r2);

  bool match = (r0 == r1) && (r1 == r2);
  std::println("All match: {}", match ? "YES" : "NO");

  return match ? 0 : 1;
}
