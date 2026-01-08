/*
 * Provided under CC0 1.0 Universal – Public Domain Dedication license
 * Original Author: Vincent Reverdy (LAPP, France)
 * Source: CppCon 2023 "Symbolic Calculus for High-performance Computing From Scratch Using C++23"
 */

/*
 * lam.symbols:operators
 * Description: Operator overloads that provide the syntactic sugar for the library.
 * Content: +, -, *, /, ^ implementation delegating to the engine.
 * Extending Author: Colin Ford
 */

module;

import std;

export module lam.symbols:operators;
import :traits;
import :core;
import :engine;

export namespace lam::symbols
{
// some operators
template<symbolic_or_arithmetic Lhs, symbolic_or_arithmetic Rhs>
  requires(symbolic<Lhs> || symbolic<Rhs>)
constexpr auto operator+(Lhs lhs, Rhs rhs) noexcept
{
  return simplify_add(lhs, rhs);
}
template<symbolic_or_arithmetic Lhs, symbolic_or_arithmetic Rhs>
  requires(symbolic<Lhs> || symbolic<Rhs>)
constexpr auto operator-(Lhs lhs, Rhs rhs) noexcept
{
  return simplify_sub(lhs, rhs);
}
template<symbolic_or_arithmetic Lhs, symbolic_or_arithmetic Rhs>
  requires(symbolic<Lhs> || symbolic<Rhs>)
constexpr auto operator*(Lhs lhs, Rhs rhs) noexcept
{
  return simplify_mul(lhs, rhs);
}
template<symbolic_or_arithmetic Lhs, symbolic_or_arithmetic Rhs>
  requires(symbolic<Lhs> || symbolic<Rhs>)
constexpr auto operator/(Lhs lhs, Rhs rhs) noexcept
{
  return simplify_div(lhs, rhs);
}
template<symbolic Arg>
constexpr auto operator-(Arg arg) noexcept
{
  return simplify_neg(arg);
}

template<symbolic_or_arithmetic Lhs, symbolic_or_arithmetic Rhs>
  requires(symbolic<Lhs> || symbolic<Rhs>)
constexpr auto operator^(Lhs lhs, Rhs rhs) noexcept
{
  return simplify_pow(lhs, rhs);
}
} // end namespace lam::symbols
