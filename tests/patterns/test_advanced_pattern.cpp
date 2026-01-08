/*
 * test_advanced_pattern.cpp
 * part of test suite for lam.symbols
 * Extending Author: Colin Ford
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info
 */

import std;
import lam.symbols;
using namespace lam::symbols;

// Helper to check if T is exactly Type
template<typename T, typename Type>
constexpr bool is_type = std::is_same_v<std::remove_cvref_t<T>, Type>;

template<typename T>
struct DebugType;

int main()
{
  constexpr symbol x;
  constexpr symbol y;
  constexpr symbol z;

  // Case 1: Simple Cancellation (Already supported?)
  // (x + y) - y -> x
  constexpr auto simple = (x + y) - y;
  if constexpr (is_type<decltype(simple), symbol<unconstrained, x.id>>)
  {
    std::println("Case 1: (x + y) - y -> x [PASS]");
  }
  else
  {
    std::println("Case 1: (x + y) - y -> Not Simplified [FAIL]");
  }

  // Case 2: Deep Cancellation (Target)
  // ((x + y) + z) - y -> x + z
  // Current structure: Minus(Plus(Plus(x, y), z), y)
  constexpr auto deep = ((x + y) + z) - y;

  // We expect it to be some form of Plus(x, z) or similar, but definitely NOT a Minus node
  using DeepType = decltype(deep);
  // DebugType<DeepType> probe; // Uncomment to see type

  if constexpr (!is_minus_expr_v<DeepType>)
  {
    std::println("Case 2: ((x + y) + z) - y -> Simplified (Not Minus) [PASS]");
  }
  else
  {
    std::println("Case 2: ((x + y) + z) - y -> Not Simplified (Still Minus) [FAIL]");
  }

  // Case 3: Deep Left Cancellation
  // ((x + y) + z) - x -> y + z
  constexpr auto deep_left = ((x + y) + z) - x;
  using DeepLeftType = decltype(deep_left);
  if constexpr (!is_minus_expr_v<DeepLeftType>)
  {
    std::println("Case 3: ((x + y) + z) - x -> Simplified (Not Minus) [PASS]");
  }
  else
  {
    std::println("Case 3: ((x + y) + z) - x -> Not Simplified (Still Minus) [FAIL]");
  }

  // Case 4: Deep Multiplication Cancellation
  // ((x * y) * z) / y -> x * z
  constexpr auto deep_mul = ((x * y) * z) / y;
  using DeepMulType = decltype(deep_mul);


  if constexpr (!is_div_expr_v<DeepMulType>)
  {
    std::println("Case 4: ((x * y) * z) / y -> Simplified (Not Divides) [PASS]");
  }
  else
  {
    std::println("Case 4: ((x * y) * z) / y -> Not Simplified (Still Divides) [FAIL]");
  }

  return 0;
}
