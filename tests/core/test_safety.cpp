/*
 * test_safety.cpp
 * part of test suite for lam.symbols
 * Extending Author: Colin Ford
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info
 */

import std;
import lam.symbols;
using namespace lam::symbols;

int main()
{
  constexpr symbol x;

  // Test 1: Safety check - Difference of stateful expressions with same type
  // x + 1.0 has type symbolic_expression<plus, symbol, double>
  // x + 2.0 has type symbolic_expression<plus, symbol, double>
  auto f1 = x + 1.0;
  auto f2 = x + 2.0;

  // If strict type-based equality is used blindly, f1 and f2 might be considered "equal" types,
  // and thus f1 - f2 might simplify to 0.
  // Correct behavior: should return a symbolic expression (or -1.0 if we had advanced simplification, but definitely
  // not 0).
  auto diff = f1 - f2;

  // Evaluate to check correctness
  double res = diff(x = 10.0);
  // (10+1) - (10+2) = 11 - 12 = -1

  if (std::abs(res - (-1.0)) < 1e-9)
  {
    std::println("PASS: (x+1.0) - (x+2.0) evaluated to -1.0");
  }
  else
  {
    std::println("FAIL: (x+1.0) - (x+2.0) evaluated to {}, expected -1.0", res);
    // If it simplified to 0, res would be 0.
    if (std::abs(res) < 1e-9)
    {
      std::println("CRITICAL FAILURE: Incorrectly simplified distinctive values to 0!");
    }
    return 1;
  }

  // Test 2: Stateless check - (x+y) - (x+y)
  // This is purely symbolic and stateless. It SHOULD simplify to 0.
  constexpr symbol y;
  auto s1 = x + y;
  auto s2 = x + y;
  auto s_diff = s1 - s2;

  using DiffType = decltype(s_diff);
  // We want this to be constant_symbol<0>
  if constexpr (std::is_same_v<DiffType, constant_symbol<0>>)
  {
    std::println("PASS: (x+y) - (x+y) simplified to constant_symbol<0>");
  }
  else
  {
    std::println("PARTIAL FAIL: (x+y) - (x+y) did NOT simplify to 0 (This is the feature we want to add)");
    // This is expected to fail currently until we add is_stateless support?
    // Or did I already verify this failed in previous interaction?
    // Previous interaction showed (x+y)-y failed. (x+y)-(x+y) probably also fails.
  }

  return 0;
}
