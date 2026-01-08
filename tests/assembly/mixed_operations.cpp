/*
 * mixed_operations.cpp
 * part of test suite for lam.symbols
 * Assembly comparison for mixed +, -, * operations
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info */

import std;
import lam.symbols;
using namespace lam::symbols;

// Case 1: Cancellation
// f(x,y) = (x + y) - x should simplify to y

__attribute__((noinline)) double cancel_expr(double val_x, double val_y)
{
  constexpr symbol x;
  constexpr symbol y;
  constexpr auto f = (x + y) - x; // -> y
  return f(x = val_x, y = val_y);
}

__attribute__((noinline)) double identity_y(double val_x, double val_y)
{
  constexpr symbol x;
  constexpr symbol y;
  constexpr auto g = y;
  return g(x = val_x, y = val_y);
}

// Case 2: Distribution / Collection
// f(x) = 2*x + 3*x should simplify to 5*x

__attribute__((noinline)) double collect_expr(double val)
{
  constexpr symbol x;
  constexpr constant_symbol<2> two;
  constexpr constant_symbol<3> three;
  constexpr auto f = (two * x) + (three * x); // -> 5 * x
  return f(x = val);
}

__attribute__((noinline)) double mul_five(double val)
{
  constexpr symbol x;
  constexpr constant_symbol<5> five;
  constexpr auto g = five * x;
  return g(x = val);
}

// Case 3: Complex Cancellation
// f(x,y) = (x + y) - (y + x) should simplify to 0

__attribute__((noinline)) double zero_expr(double val_x, double val_y)
{
  constexpr symbol x;
  constexpr symbol y;
  constexpr auto f = (x + y) - (y + x); // -> 0
  // Note: constant_zero below doesn't take args, so zero_expr shouldn't technically need them if simplified
  // But evaluating it might still look up bindings if not fully optimized away at runtime logic level
  // (though compilation to 0 should ignore args).
  return f(x = val_x, y = val_y);
}

__attribute__((noinline)) double constant_zero(double val_x, double val_y)
{
  constexpr auto g = constant_symbol<0>{};
  return g(); // 0
}

int main()
{
  volatile double x = 10.0;
  volatile double y = 20.0;

  double r1 = cancel_expr(x, y);
  double r2 = identity_y(x, y);
  std::println("(x+y)-x = {}, y = {} [Match: {}]", r1, r2, (r1 == r2));

  double r3 = collect_expr(x);
  double r4 = mul_five(x);
  std::println("2x+3x = {}, 5x = {} [Match: {}]", r3, r4, (r3 == r4));

  double r5 = zero_expr(x, y);
  double r6 = constant_zero(x, y);
  std::println("(x+y)-(y+x) = {}, 0 = {} [Match: {}]", r5, r6, (r5 == r6));

  return 0;
}
