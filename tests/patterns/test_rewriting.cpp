/*
 * test_rewriting.cpp
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
  constexpr symbol y;
  constexpr symbol z;

  // Formula: f = x + 2
  auto f = x + 2.0;

  // Rewriting: replace x with (y / z)
  // Expected: (y/z) + 2.0
  auto g = f(x = y / z);

  // Evaluate g with y=4, z=2 -> (4/2) + 2 = 4
  double result = g(y = 4.0, z = 2.0);

  std::println("Result: {}", result);

  if (std::abs(result - 4.0) > 1e-9)
  {
    std::println("FAIL: Expected 4.0, got {}", result);
    return 1;
  }

  std::println("SUCCESS 1: Basic Rewriting");

  // 2. Nested rewriting
  // Formula: h = x * y
  constexpr auto h = x * y;
  // bound x = (y + z)
  auto h2 = h(x = y + z);
  // Expected: (y + z) * y

  // Evaluate with y=2, z=3 -> (2+3)*2 = 10
  double val2 = h2(y = 2.0, z = 3.0);
  std::println("Result 2: {}", val2);

  if (std::abs(val2 - 10.0) > 1e-9)
  {
    std::println("FAIL 2: Expected 10.0, got {}", val2);
    return 1;
  }
  std::println("SUCCESS 2: Nested Rewriting");

  // 3. Mixed Rewriting and Substitution
  // Formula: k = x + y + z
  constexpr auto k = x + y + z;

  // Rewriting x = y^2 (symbolic), Substitution z = 5.0 (numeric)
  auto k2 = k(x = y * y, z = 5.0);
  // Expected: (y*y) + y + 5.0

  // Evaluate with y=3 -> 9 + 3 + 5 = 17
  double val3 = k2(y = 3.0);
  std::println("Result 3: {}", val3);

  if (std::abs(val3 - 17.0) > 1e-9)
  {
    std::println("FAIL 3: Expected 17.0, got {}", val3);
    return 1;
  }
  std::println("SUCCESS 3: Mixed Rewriting");

  std::println("ALL SUCCESS");
  return 0;
}
