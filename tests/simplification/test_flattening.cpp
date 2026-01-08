/*
 * test_flattening.cpp
 * part of test suite for lam.symbols
 * Extending Author: Colin Ford
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info
 */

import std;
import lam.symbols;
using namespace lam::symbols;

template<typename T>
struct get_arity
{
  static constexpr std::size_t value = 0;
};

template<typename Op, typename... Terms>
struct get_arity<symbolic_expression<Op, Terms...>>
{
  static constexpr std::size_t value = sizeof...(Terms);
};

int main()
{
  constexpr symbol x;
  constexpr symbol y;
  constexpr symbol z;

  // Test 1: Addition Flattening
  // (x + y) + z -> Plus(x, y, z)
  constexpr auto sum_expr = (x + y) + z;

  // Check it is still a Plus expression
  static_assert(is_plus_expr_v<decltype(sum_expr)>, "Should be a Plus expression");

  // Check arity
  constexpr std::size_t sum_arity = get_arity<std::remove_cvref_t<decltype(sum_expr)>>::value;
  std::println("Sum Arity: {}", sum_arity);

  if constexpr (sum_arity == 3)
    std::println("Sum Flattening: ((x+y)+z) -> Plus(x,y,z) [PASS]");
  else
    std::println("Sum Flattening: ((x+y)+z) -> Arity {} [FAIL]", sum_arity);

  // Test 2: Multiplication Flattening
  // (x * y) * z -> Mul(x, y, z)
  constexpr auto mul_expr = (x * y) * z;
  static_assert(is_mul_expr_v<decltype(mul_expr)>, "Should be a Mul expression");

  constexpr std::size_t mul_arity = get_arity<std::remove_cvref_t<decltype(mul_expr)>>::value;
  std::println("Mul Arity: {}", mul_arity);

  if constexpr (mul_arity == 3)
    std::println("Mul Flattening: ((x*y)*z) -> Mul(x,y,z) [PASS]");
  else
    std::println("Mul Flattening: ((x*y)*z) -> Arity {} [FAIL]", mul_arity);

  return 0;
}
