/*
 * power_ext_assembly.cpp
 * part of test suite for lam.symbols
 * Assembly comparison for extended power rules
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info */

import std;
import lam.symbols;
using namespace lam::symbols;

// Case 1: Power of Power
// f(x) = (x^2)^3 should be identical to g(x) = x^6

__attribute__((noinline)) double pow_pow_expr(double val)
{
  constexpr symbol x;
  constexpr constant_symbol<2> two;
  constexpr constant_symbol<3> three;
  constexpr auto f = (x ^ two) ^ three; // (x^2)^3 -> x^6
  return f(x = val);
}

__attribute__((noinline)) double pow_six_expr(double val)
{
  constexpr symbol x;
  constexpr constant_symbol<6> six;
  constexpr auto g = x ^ six; // x^6
  return g(x = val);
}

// Case 2: Division of Powers
// f(x) = x^5 / x^2 should be identical to g(x) = x^3

__attribute__((noinline)) double div_pow_expr(double val)
{
  constexpr symbol x;
  constexpr constant_symbol<5> five;
  constexpr constant_symbol<2> two;
  constexpr auto f = (x ^ five) / (x ^ two); // x^5 / x^2 -> x^3
  return f(x = val);
}

__attribute__((noinline)) double pow_three_expr(double val)
{
  constexpr symbol x;
  constexpr constant_symbol<3> three;
  constexpr auto g = x ^ three; // x^3
  return g(x = val);
}


int main()
{
  volatile double input = 2.0;

  // Check values match first
  double r1 = pow_pow_expr(input);
  double r2 = pow_six_expr(input);
  std::println("(x^2)^3 = {}, x^6 = {} [Match: {}]", r1, r2, (r1 == r2));

  double r3 = div_pow_expr(input);
  double r4 = pow_three_expr(input);
  std::println("x^5/x^2 = {}, x^3 = {} [Match: {}]", r3, r4, (r3 == r4));

  return 0;
}
