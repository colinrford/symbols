/*
 * Provided under CC0 1.0 Universal – Public Domain Dedication license
 * Original Author: Vincent Reverdy (LAPP, France)
 * Source: CppCon 2023 "Symbolic Calculus for High-performance Computing From Scratch Using C++23"
 */

/*
 * lam.symbols:traits
 * Description: Low-level metaprogramming helpers and concept definitions.
 * Content: remove_cvref extensions, index_constant, is_symbolic concepts.
 * Extending Author: Colin Ford
 */

module;

import std;

export module lam.symbols:traits;

export namespace lam::symbols
{
// remove lvalue references
template <typename T>
struct remove_lvalue_reference : std::type_identity<T> {};

template <typename T>
requires std::is_lvalue_reference_v<T>
struct remove_lvalue_reference<T>
  : std::type_identity<std::remove_reference_t<T>>
{};

template <typename T>
using remove_lvalue_reference_t = remove_lvalue_reference <T>::type;
// remove rvalue references
template <typename T>
struct remove_rvalue_reference: std::type_identity<T> {};

template <typename T>
requires std::is_rvalue_reference_v<T>
struct remove_rvalue_reference<T>
  : std::type_identity<std::remove_reference_t<T>>
{};

template <typename T>
using remove_rvalue_reference_t = remove_rvalue_reference<T>::type;
// end removing references stuff
// const-qualifying the non-referenced type
// clang-format off
template <typename T>
struct requalify_as_const
  : std::conditional
    <
      std::is_lvalue_reference_v <T>,
      std::add_lvalue_reference_t
      <
        std::add_const_t<std::remove_reference_t<T>>
      >,
      std::conditional_t
      <
        std::is_rvalue_reference_v <T>,
        std::add_rvalue_reference_t
        <
          std::add_const_t<std::remove_reference_t <T>>
        >,
        std::add_const_t<T>
      >
    >
{};
// clang-format on

template <typename T>
using requalify_as_const_t = requalify_as_const<T>::type;

template <typename T>
struct requalify_as_volatile;
template <typename T>
using requalify_as_volatile_t = requalify_as_volatile<T>::type;

template <typename T>
struct requalify_as_cv;
template <typename T>
using requalify_as_cv_t = requalify_as_cv<T>::type;
// end const-qualifying non-referenced type
// Index constant type
template <std::size_t I>
struct index_constant : std::integral_constant<std::size_t, I> {};

/*
 *  architecture
 */
// Type trait
template <typename>
struct is_symbolic : std::false_type {};
// Variable template
template <typename T>
inline constexpr bool is_symbolic_v = is_symbolic<T>::value;
// Concept
template <typename T>
concept symbolic = is_symbolic_v<T>;

} // end namespace lam::symbols
