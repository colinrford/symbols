/*
 * test_coverage_boundary.cpp
 * part of test suite for lam.symbols
 * Extending Author: Colin Ford
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info
 */

import std;
import lam.symbols;
using namespace lam::symbols;

template<typename T, typename Op>
struct is_op : std::false_type
{};
template<typename Op, typename... Args>
struct is_op<symbolic_expression<Op, Args...>, Op> : std::true_type
{};

template<typename T>
struct DebugType;

int main()
{
  constexpr symbol x;
  constexpr symbol y;
  constexpr symbol z;

  // Supported: ((x + y) + z) - y
  constexpr auto supported = ((x + y) + z) - y;
  if constexpr (!is_op<decltype(supported), std::minus<void>>::value)
  {
    std::println("Supported: ((x + y) + z) - y -> Optimized [PASS]");
  }
  else
  {
    std::println("Supported: ((x + y) + z) - y -> Failed [FAIL]");
  }

  // Boundary Check: ((x - y) - z) + y
  // This requires traversing a chain of Minuses. Minus is not associative.
  // ((x - y) - z) + y => (x - z)
  constexpr auto boundary = ((x - y) - z) + y;

  // We expect this to FAIL currently (remain a Plus expression)
  if constexpr (is_op<std::remove_cvref_t<decltype(boundary)>, std::plus<void>>::value)
  {
    std::println("Boundary: ((x - y) - z) + y -> Not Simplified [EXPECTED GAP]");
  }
  else
  {
    std::println("Boundary: ((x - y) - z) + y -> Optimized [UNEXPECTED PASS]");
  }

  return 0;
}
