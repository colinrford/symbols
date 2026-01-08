/*
 * assembly_check.cpp
 * part of test suite for lam.symbols
 * Assembly comparison: f(x) = x vs g(x) = (x + y) - y
 * Both should produce identical assembly when simplified.
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info 
 */

import lam.symbols;

using namespace lam::symbols;

// Original simple check
extern "C" double compute_expression(double input)
{
  constexpr symbol x;
  // (x + x) -> 2 * x
  auto expr = x + x;
  return expr(x = input);
}

// Advanced Check: Deep Cancellation
// ((x + y) + z) - y
// Should optimize to: x + z
// The 'y' argument should be completely ignored (no load, no add/sub).
extern "C" double compute_advanced(double x_val, double y_val, double z_val)
{
  constexpr symbol x;
  constexpr symbol y;
  constexpr symbol z;

  // This looks like a complex tree of Minus(Plus(Plus(x,y), z), y)
  // But our new engine should rewrite it to Plus(x, z) at compile time.
  auto expr = ((x + y) + z) - y;

  return expr(x = x_val, y = y_val, z = z_val);
}
