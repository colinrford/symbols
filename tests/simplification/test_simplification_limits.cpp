/*
 * test_simplification_limits.cpp
 * part of test suite for lam.symbols
 * Extending Author: Colin Ford
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info
 */

import std;
import lam.symbols;
using namespace lam::symbols;

template<typename T>
void print_type()
{
  std::println("Type: {}", typeid(T).name());
}

int main()
{
  constexpr symbol x;
  constexpr symbol y;

  // Test 1: (x - x) -> 0
  auto expr1 = x - x;
  using Type1 = decltype(expr1);
  constexpr bool is_zero1 = std::is_same_v<Type1, constant_symbol<0>>;

  if (is_zero1)
  {
    std::println("PASS: x - x simplified to constant_symbol<0>");
  }
  else
  {
    std::println("FAIL: x - x did NOT simplify to constant_symbol<0>");
    print_type<Type1>();
  }

  // Test 2: (x + 0) -> x
  auto expr2 = x + 0.0;
  // Note: 0.0 is double, so we might get symbolic_expression or just symbol?
  // The simplification rule for arithmetic types says: if x+0 with double 0, it might return x (as symbol).
  // But wait, the simplify_add implementation checks for constant_symbol<0>.
  // 0.0 is a double, not constant_symbol<0>.
  // So x + 0.0 might NOT simplify at AST level unless it's constant_symbol<0>.

  constexpr constant_symbol<0> zero;
  auto expr2b = x + zero;
  using Type2b = decltype(expr2b);
  constexpr bool is_x = std::is_same_v<std::remove_cvref_t<Type2b>, std::remove_cvref_t<decltype(x)>>;

  if (is_x)
  {
    std::println("PASS: x + constant_symbol<0> simplified to symbol x");
  }
  else
  {
    std::println("FAIL: x + constant_symbol<0> did NOT simplify to symbol x");
    print_type<Type2b>();
  }

  // Test 3: (x * 1) -> x
  constexpr constant_symbol<1> one;
  auto expr3 = x * one;
  using Type3 = decltype(expr3);
  constexpr bool is_x3 = std::is_same_v<std::remove_cvref_t<Type3>, std::remove_cvref_t<decltype(x)>>;

  if (is_x3)
  {
    std::println("PASS: x * constant_symbol<1> simplified to symbol x");
  }
  else
  {
    std::println("FAIL: x * constant_symbol<1> did NOT simplify to symbol x");
    print_type<Type3>();
  }

  // Test 4: (x + y) - y -> x
  auto expr4 = (x + y) - y;
  using Type4 = decltype(expr4);
  constexpr bool is_x4 = std::is_same_v<std::remove_cvref_t<Type4>, std::remove_cvref_t<decltype(x)>>;

  if (is_x4)
  {
    std::println("PASS: (x + y) - y simplified to symbol x");
  }
  else
  {
    std::println("FAIL: (x + y) - y did NOT simplify to symbol x");
    print_type<Type4>();
  }

  return 0;
}
