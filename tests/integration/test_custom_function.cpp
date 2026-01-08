/*
 * test_custom_function.cpp
 * part of test suite for lam.symbols
 * Extending Author: Colin Ford
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info
 */

import std;
import lam.symbols;

using namespace lam::symbols;

// Define a custom Sin operator (stateless)
struct SinOp
{
  template<typename T>
  constexpr auto operator()(T&& arg) const
  {
    return std::sin(std::forward<T>(arg));
  }
};

// Make it a valid symbolic operator?
// The engine currently supports arbitrary operators in symbolic_expression.
// But we need a helper to create the expression.

template<typename Expr>
constexpr auto sin(Expr&& expr)
{ // If it's a constant, evaluate immediately
  if constexpr (is_constant_symbol_v<Expr>)
    return constant_symbol<std::sin(Expr::value)>{};
  else
    return symbolic_expression<SinOp, std::remove_cvref_t<Expr>>{std::forward<Expr>(expr)};
}

int main()
{
  constexpr symbol x;

  // Test 1: sin(x)
  constexpr auto mysin = sin(x);

  // Test 2: sin(x) - sin(x) -> 0 ?
  // The simplified_sub logic checks if operands are the "same symbolic value".
  // Types: symbolic_expression<SinOp, symbol...> vs symbolic_expression<SinOp, symbol...>
  // They are same type.
  // Are they stateless? SinOp is empty. Symbol is stateless. So yes.

  constexpr auto diff = mysin - mysin;

  constexpr bool is_zero = is_constant_symbol_v<decltype(diff)> && (decltype(diff)::value == 0);

  if constexpr (is_zero)
    std::println("sin(x) - sin(x) simplified to 0!");
  else
    std::println("sin(x) - sin(x) DID NOT simplify to 0.");

  // Test 3: sin(x) + sin(x) -> 2 * sin(x) ?
  constexpr auto sum = mysin + mysin;
  // We expect 2 * sin(x)
  // Structure: symbolic_expression<multiplies, constant<2>, symbolic_expression<SinOp, ...>>

  constexpr bool is_double = is_mul_expr_v<decltype(sum)> && is_constant_symbol_v<get_lhs_t<decltype(sum)>> &&
                             (get_lhs_t<decltype(sum)>::value == 2);

  if constexpr (is_double)
    std::println("sin(x) + sin(x) simplified to 2 * sin(x)!");
  else
    std::println("sin(x) + sin(x) DID NOT simplify.");

  // Test 4: sin(x) * sin(x) -> sin(x)^2 ?
  constexpr auto prod = mysin * mysin;
  constexpr bool is_square = is_power_expr_v<decltype(prod)> && (get_rhs_t<decltype(prod)>::value == 2);

  if constexpr (is_square)
    std::println("sin(x) * sin(x) simplified to sin(x)^2!");
  else
    std::println("sin(x) * sin(x) DID NOT simplify.");

  // Test 5: sin(x) / sin(x) -> 1 ?
  constexpr auto div = mysin / mysin;
  constexpr bool is_one = is_constant_symbol_v<decltype(div)> && (decltype(div)::value == 1);

  if constexpr (is_one)
    std::println("sin(x) / sin(x) simplified to 1!");
  else
    std::println("sin(x) / sin(x) DID NOT simplify.");

  return (is_zero && is_double && is_square && is_one) ? 0 : 1;
}
