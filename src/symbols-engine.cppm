/*
 * Provided under CC0 1.0 Universal – Public Domain Dedication license
 * Original Author: Vincent Reverdy (LAPP, France)
 * Source: CppCon 2023 "Symbolic Calculus for High-performance Computing From Scratch Using C++23"
 */

/*
 * lam.symbols:engine
 * Description: The structural logic and simplification rules.
 * Content: symbolic_expression (AST), formula, simplify_expression (Pattern Matching).
 * Extensibility: This is the primary location for adding new rewriting rules or calculus operations.
 * Extending Author: Colin Ford
 */

module;

import std;

export module lam.symbols:engine;
import :traits;
import :core;

export namespace lam::symbols
{

/*
 *  Architecture (Expression)
 */

// Forward declaration for mixed-mode operators support
template<typename T>
struct is_symbolic_expression : std::false_type
{};
// Forward declaration of symbolic_expression
template<typename Operator, typename... Terms>
struct symbolic_expression;
template<typename Op, typename... Terms>
struct is_symbolic_expression<symbolic_expression<Op, Terms...>> : std::true_type
{};

template<typename T>
concept symbolic_or_arithmetic = symbolic<T> || std::is_arithmetic_v<T>;

template<symbolic Expression>
struct formula
{
  using expression_type = Expression;
  Expression expression;

  constexpr formula(Expression expr) noexcept : expression(expr) {};

  // Call operator where substitution happens
  template<class... Args>
  constexpr auto operator()(Args... args) const noexcept
  {
    // Create substitution
    auto sub = substitution(args...);
    // Evaluate expression with substitution
    return expression(sub);
  }
};

/*
 *  Construction
 */

// Helper to evaluate a term: if symbolic, call it; otherwise return value
template<typename Term, typename Substitution>
constexpr auto evaluate_term(const Term& term, const Substitution& s)
{
  if constexpr (is_symbolic_v<std::remove_cvref_t<Term>>)
    return term(s);
  else
    return term;
}


/*
 *  Simplification
 */

// Main simplification dispatcher forward declaration
template<typename Operator, typename Lhs, typename Rhs>
constexpr auto simplify_expression(const Operator& op, Lhs&& lhs, Rhs&& rhs);

// Unary simplification dispatcher forward declaration
template<typename Operator, typename Arg>
constexpr auto simplify_expression(const Operator& op, Arg&& arg);


// The class for symbolic expressions
template<typename Operator, typename... Terms>
struct symbolic_expression
{
  std::tuple<Terms...> terms;

  constexpr symbolic_expression(Terms... t) : terms(t...) {}

  // Structural equality
  friend constexpr bool operator==(const symbolic_expression& lhs, const symbolic_expression& rhs)
  {
    return lhs.terms == rhs.terms;
  }

  // Recursive helper for N-ary evaluation
  template<std::size_t I, typename Acc, typename Subst>
  constexpr auto evaluate_and_fold(Acc&& acc, const Subst& s) const
  {
    if constexpr (I >= sizeof...(Terms))
      return acc;
    else
    {
      // Evaluate next term
      auto term_val = evaluate_term(std::get<I>(terms), s);
      // Simplify accumulator + term
      auto new_acc = simplify_expression(Operator{}, std::forward<Acc>(acc), term_val);
      // Recurse
      return evaluate_and_fold<I + 1>(std::move(new_acc), s);
    }
  }

  template<class... Binders>
  constexpr auto operator()(const substitution<Binders...>& s) const noexcept
  {
    if constexpr (sizeof...(Terms) == 1)
    {
       return simplify_expression(Operator{}, evaluate_term(std::get<0>(terms), s));
    }
    else
    {
       // Start with first term evaluated
       auto first_val = evaluate_term(std::get<0>(terms), s);
       // Fold the rest starting from index 1
       return evaluate_and_fold<1>(std::move(first_val), s);
    }
  }

  // Convenience operator (moved inside to avoid out-of-line deduction issues)
  template<class... Args>
  constexpr auto operator()(Args... args) const noexcept
  {
    return (*this)(substitution(args...));
  }
};

// Making it symbolic
template<typename Operator, typename... Terms>
struct is_symbolic<symbolic_expression<Operator, Terms...>> : std::true_type
{};

/*
 *  Simplification Logic (Strict Type-Based)
 */

// Trait: Check if type is structurally zero (constant_symbol<0>)
template<typename T>
struct is_structural_zero : std::false_type
{};
template<>
struct is_structural_zero<constant_symbol<0>> : std::true_type
{};
template<typename T>
constexpr bool is_structural_zero_v = is_structural_zero<std::remove_cvref_t<T>>::value;

// Trait: Check if type is structurally one (constant_symbol<1>)
template<typename T>
struct is_structural_one : std::false_type
{};
template<>
struct is_structural_one<constant_symbol<1>> : std::true_type
{};
template<typename T>
constexpr bool is_structural_one_v = is_structural_one<std::remove_cvref_t<T>>::value;

// Trait: Check if type is "Stateless" (contains no runtime data like int, double)
template<typename T>
struct is_stateless : std::false_type
{};
template<class Trait, auto Id>
struct is_stateless<symbol<Trait, Id>> : std::true_type
{};
template<auto Val>
struct is_stateless<constant_symbol<Val>> : std::true_type
{};
// Symbolic expression is stateless if all terms are stateless
template<typename Op, typename... Terms>
struct is_stateless<symbolic_expression<Op, Terms...>>
  : std::bool_constant<(is_stateless<std::remove_cvref_t<Terms>>::value && ...)>
{};
template<typename T>
constexpr bool is_stateless_v = is_stateless<std::remove_cvref_t<T>>::value;

// Trait: Check if two types represent the exact same symbolic value
// If T and U are the same type AND T is stateless, then they are equal values.
template<typename T, typename U>
struct are_same_symbolic_value : std::false_type
{};
template<typename T>
struct are_same_symbolic_value<T, T> : std::bool_constant<is_stateless_v<T>>
{};

template<typename T, typename U>
constexpr bool are_same_symbolic_value_v =
  are_same_symbolic_value<std::remove_cvref_t<T>, std::remove_cvref_t<U>>::value;

/*
 *  Expression Decomposition Traits
 *  These enable compile-time pattern matching on symbolic expression structure
 */

// Forward declaration of power operator (defined later)
template<typename T>
struct power;

// Extract operator from a symbolic_expression
template<typename T>
struct expression_operator
{ using type = void; };
template<typename Op, typename... Terms>
struct expression_operator<symbolic_expression<Op, Terms...>>
{ using type = Op; };
template<typename T>
using expr_op_t = typename expression_operator<std::remove_cvref_t<T>>::type;

// Extract left operand (for binary expressions)
template<typename T>
struct expression_lhs
{ using type = void; };
template<typename Op, typename L, typename R>
struct expression_lhs<symbolic_expression<Op, L, R>> 
{ using type = L; };
template<typename T>
using expr_lhs_t = typename expression_lhs<std::remove_cvref_t<T>>::type;

// Extract right operand (for binary expressions)
template<typename T>
struct expression_rhs
{ using type = void; };
template<typename Op, typename L, typename R>
struct expression_rhs<symbolic_expression<Op, L, R>>
{ using type = R; };
template<typename T>
using expr_rhs_t = typename expression_rhs<std::remove_cvref_t<T>>::type;

// Check if expression is a specific operator type
template<typename T, typename Op>
struct is_expr_with_op : std::false_type
{};
template<typename Op, typename... Terms>
struct is_expr_with_op<symbolic_expression<Op, Terms...>, Op> : std::true_type
{};

template<typename T>
constexpr bool is_plus_expr_v = is_expr_with_op<std::remove_cvref_t<T>, std::plus<void>>::value;
template<typename T>
constexpr bool is_minus_expr_v = is_expr_with_op<std::remove_cvref_t<T>, std::minus<void>>::value;
template<typename T>
constexpr bool is_mul_expr_v = is_expr_with_op<std::remove_cvref_t<T>, std::multiplies<void>>::value;
template<typename T>
constexpr bool is_div_expr_v = is_expr_with_op<std::remove_cvref_t<T>, std::divides<void>>::value;
template<typename T>
constexpr bool is_power_expr_v = is_expr_with_op<std::remove_cvref_t<T>, power<void>>::value;
template<typename T>
constexpr bool is_negate_expr_v = is_expr_with_op<std::remove_cvref_t<T>, std::negate<void>>::value;

// Unified Structural Accessors
template<typename T>
struct get_lhs
{ using type = void; };
template<typename T>
  requires is_symbolic_expression<T>::value
struct get_lhs<T>
{ using type = expr_lhs_t<T>; };
template<typename T>
using get_lhs_t = typename get_lhs<std::remove_cvref_t<T>>::type;

template<typename T>
struct get_rhs
{ using type = void; };
template<typename T>
  requires is_symbolic_expression<T>::value
struct get_rhs<T>
{ using type = expr_rhs_t<T>; };
template<typename T>
using get_rhs_t = typename get_rhs<std::remove_cvref_t<T>>::type;

template<typename T>
constexpr bool has_lhs_v = !std::is_void_v<get_lhs_t<T>>;

template<typename T>
constexpr decltype(auto) get_lhs_val(T&& expr)
{ return std::get<0>(std::forward<T>(expr).terms); }

template<typename T>
constexpr decltype(auto) get_rhs_val(T&& expr)
{
  if constexpr (std::tuple_size_v<decltype(std::remove_cvref_t<T>::terms)> > 1)
    return std::get<1>(std::forward<T>(expr).terms);
  else
    return std::get<0>(std::forward<T>(expr).terms); // Fallback for single term? Unlikely used here.
}

// Generalized Recursive Cancellation
// Matches if Rhs is found deeply nested in Lhs (which must be AssociativeOp).
template<typename Lhs, typename Rhs, typename AssociativeOp>
struct is_deep_cancellation;

// clang-format off
template <typename Lhs, typename Rhs, typename AssociativeOp>
struct is_deep_cancellation 
    : std::conditional_t<
        // Base Case 1: (A op B) - B [Right side match]
        are_same_symbolic_value_v<get_rhs_t<Lhs>, Rhs>,
        std::true_type,
        std::conditional_t<
             // Base Case 2: (A op B) - A [Left side match]
             are_same_symbolic_value_v<get_lhs_t<Lhs>, Rhs>,
             std::true_type,
             std::conditional_t<
                  // Recursive Case: ((... op B) op C) - B -> Recurse on Left
                  is_expr_with_op<std::remove_cvref_t<Lhs>, AssociativeOp>::value,
                  is_deep_cancellation<get_lhs_t<Lhs>, Rhs, AssociativeOp>,
                  std::false_type
             >
        >
      > 
{};
// clang-format on

template<typename Lhs, typename Rhs, typename AssociativeOp>
constexpr bool is_deep_cancellation_v = is_deep_cancellation<Lhs, Rhs, AssociativeOp>::value;

// Forward declarations
template<typename Lhs, typename Rhs>
constexpr auto simplify_add(Lhs&& lhs, Rhs&& rhs);
template<typename Lhs, typename Rhs>
constexpr auto simplify_mul(Lhs&& lhs, Rhs&& rhs);

// Trait: Check if type is constant_symbol<V>
template<typename T>
struct is_constant_symbol : std::false_type
{};
template<auto V>
struct is_constant_symbol<constant_symbol<V>> : std::true_type
{};
template<typename T>
constexpr bool is_constant_symbol_v = is_constant_symbol<std::remove_cvref_t<T>>::value;

// Term Analysis Traits
template<typename T>
struct term_traits
{
  static constexpr auto get_coeff(const T&) { return constant_symbol<1>{}; }
  static constexpr const T& get_base(const T& t) { return t; }
};
template<typename T>
  requires is_constant_symbol_v<T>
struct term_traits<T>
{
  static constexpr auto get_coeff(const T& t) { return t; }
  static constexpr auto get_base(const T&) { return constant_symbol<1>{}; }
};
template<typename... Args>
struct term_traits<symbolic_expression<std::multiplies<void>, Args...>>
{
  using Expr = symbolic_expression<std::multiplies<void>, Args...>;
  static constexpr bool first_is_const = is_constant_symbol_v<std::tuple_element_t<0, std::tuple<Args...>>>;

  static constexpr auto get_coeff(const Expr& expr)
  {
    if constexpr (first_is_const)
      return std::get<0>(expr.terms);
    else
      return constant_symbol<1>{};
  }
  static constexpr auto get_base(const Expr& expr)
  {
    if constexpr (first_is_const)
    {
      if constexpr (sizeof...(Args) == 2)
        return std::get<1>(expr.terms);
      else
        return expr;
    }
    else
      return expr;
  }
};

// Tuple Helpers
template<std::size_t I, std::size_t SkipIndex>
struct remove_helper
{
  template<typename Tuple>
  static constexpr auto apply(Tuple&& t)
  { return std::make_tuple(std::get<I>(std::forward<Tuple>(t))); }
};
template<std::size_t I>
struct remove_helper<I, I>
{
  template<typename Tuple>
  static constexpr auto apply(Tuple&&)
  { return std::tuple<>{}; }
};

template<typename Tuple, std::size_t SkipIndex, std::size_t... I>
constexpr auto tuple_remove_impl(Tuple&& t, std::index_sequence<I...>)
{
  return std::tuple_cat(remove_helper<I, SkipIndex>::apply(std::forward<Tuple>(t))...);
}

template<std::size_t I, std::size_t ReplaceIndex>
struct replace_helper
{
  template<typename Tuple, typename NewElem>
  static constexpr auto apply(Tuple&& t, NewElem&&)
  { return std::make_tuple(std::get<I>(std::forward<Tuple>(t))); }
};
template<std::size_t I>
struct replace_helper<I, I>
{
  template<typename Tuple, typename NewElem>
  static constexpr auto apply(Tuple&&, NewElem&& elem)
  { return std::make_tuple(std::forward<NewElem>(elem)); }
};

template<typename Tuple, std::size_t ReplaceIndex, typename NewElem, std::size_t... I>
constexpr auto tuple_replace_impl(Tuple&& t, NewElem&& elem, std::index_sequence<I...>)
{
  return std::tuple_cat(replace_helper<I, ReplaceIndex>::apply(std::forward<Tuple>(t), std::forward<NewElem>(elem))...);
}

// Recursive Merge
template<std::size_t I = 0, typename Tuple, typename Term>
constexpr auto merge_term_into_tuple(Tuple&& tuple, Term&& term)
{
  if constexpr (I >= std::tuple_size_v<std::remove_cvref_t<Tuple>>)
    return std::tuple_cat(std::forward<Tuple>(tuple), std::make_tuple(std::forward<Term>(term)));
  else
  {
    auto& current = std::get<I>(tuple);
    using CurrTraits = term_traits<std::remove_cvref_t<decltype(current)>>;
    using TermTraits = term_traits<std::remove_cvref_t<Term>>;

    auto base1 = CurrTraits::get_base(current);
    auto base2 = TermTraits::get_base(term);

    if constexpr (are_same_symbolic_value_v<decltype(base1), decltype(base2)>)
    {
      auto new_coeff = simplify_add(CurrTraits::get_coeff(current), TermTraits::get_coeff(term));

      if constexpr (is_structural_zero_v<decltype(new_coeff)>)
        return tuple_remove_impl<Tuple, I>(std::forward<Tuple>(tuple),
                                           std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>{});
      else
      {
        auto new_term = simplify_mul(new_coeff, base1);
        return tuple_replace_impl<Tuple, I>(std::forward<Tuple>(tuple), std::move(new_term),
                                            std::make_index_sequence<std::tuple_size_v<std::remove_cvref_t<Tuple>>>{});
      }
    }
    else
      return merge_term_into_tuple<I + 1>(std::forward<Tuple>(tuple), std::forward<Term>(term));
  }
}

// Helper: merge_flat_expression (Replacement for append_flat)
template<typename Op, typename Tuple1, typename Term>
constexpr auto merge_flat_expression(Tuple1&& t1, Term&& term)
{
  if constexpr (std::is_same_v<Op, std::plus<void>>)
  {
    auto merged_tuple = merge_term_into_tuple(std::forward<Tuple1>(t1), std::forward<Term>(term));
    return std::apply(
      [](auto&&... args) {
        if constexpr (sizeof...(args) == 0)
          return constant_symbol<0>{};
        else if constexpr (sizeof...(args) == 1)
          return std::get<0>(std::make_tuple(std::forward<decltype(args)>(args)...));
        else
          return symbolic_expression<Op, std::remove_cvref_t<decltype(args)>...>(std::forward<decltype(args)>(args)...);
      },
      std::move(merged_tuple));
  }
  else
  {
    // Fallback for Mul: Just append
    auto t2 = std::make_tuple(std::forward<Term>(term));
    auto combined = std::tuple_cat(std::forward<Tuple1>(t1), std::move(t2));
    return std::apply(
      [](auto&&... args) {
        return symbolic_expression<Op, std::remove_cvref_t<decltype(args)>...>(std::forward<decltype(args)>(args)...);
      },
      std::move(combined));
  }
}

// Helper: make_flat_expression (Concatenate two tuples)
template<typename Op, typename Tuple1, typename Tuple2>
constexpr auto make_flat_expression(Tuple1&& t1, Tuple2&& t2)
{
  auto combined = std::tuple_cat(std::forward<Tuple1>(t1), std::forward<Tuple2>(t2));
  return std::apply(
    [](auto&&... args) {
      return symbolic_expression<Op, std::remove_cvref_t<decltype(args)>...>(std::forward<decltype(args)>(args)...);
    },
    std::move(combined));
}

// Simplification for addition
template<typename Lhs, typename Rhs>
constexpr auto simplify_add(Lhs&& lhs, Rhs&& rhs)
{
  // Flattening Logic
  if constexpr (is_plus_expr_v<Lhs> && is_plus_expr_v<Rhs>)
    return make_flat_expression<std::plus<void>>(std::forward<Lhs>(lhs).terms, std::forward<Rhs>(rhs).terms);
  else if constexpr (is_plus_expr_v<Lhs>)
    return merge_flat_expression<std::plus<void>>(std::forward<Lhs>(lhs).terms, std::forward<Rhs>(rhs));
  else if constexpr (is_plus_expr_v<Rhs>)
    // Commutative merge: merge Lhs (as term) into Rhs (tuple)
    return merge_flat_expression<std::plus<void>>(std::forward<Rhs>(rhs).terms, std::forward<Lhs>(lhs));
  // Pattern: 0 + x -> x
  else if constexpr (is_structural_zero_v<Lhs>)
    return std::forward<Rhs>(rhs);
  else if constexpr (is_structural_zero_v<Rhs>)
    return std::forward<Lhs>(lhs);
  else if constexpr (is_constant_symbol_v<Lhs> && is_constant_symbol_v<Rhs>)
    return constant_symbol<std::remove_cvref_t<Lhs>::value + std::remove_cvref_t<Rhs>::value>{};
  else if constexpr (are_same_symbolic_value_v<Lhs, Rhs>)
    // Pattern: x + x → 2 * x
    return constant_symbol<2>{} * std::forward<Lhs>(lhs);
  // Pattern: (A - B) + B → A
  else if constexpr (is_minus_expr_v<Lhs> && are_same_symbolic_value_v<expr_rhs_t<Lhs>, Rhs>)
    return expr_lhs_t<Lhs>{};
  // Pattern: B + (A - B) → A
  else if constexpr (is_minus_expr_v<Rhs> && are_same_symbolic_value_v<Lhs, expr_rhs_t<Rhs>>)
    return expr_lhs_t<Rhs>{};
  else if constexpr (std::is_arithmetic_v<std::remove_cvref_t<Lhs>> && std::is_arithmetic_v<std::remove_cvref_t<Rhs>>)
    return lhs + rhs;
  else
    return symbolic_expression<std::plus<void>, std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>(
      std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
}

// Simplification for subtraction (Normalize to Addition)
template<typename Lhs, typename Rhs>
constexpr auto simplify_sub(Lhs&& lhs, Rhs&& rhs)
{
  if constexpr (is_structural_zero_v<Rhs>)
    return std::forward<Lhs>(lhs);
  else if constexpr (are_same_symbolic_value_v<Lhs, Rhs>)
    return constant_symbol<0>{};
  else if constexpr (is_structural_zero_v<Lhs>)
    // 0 - x -> -1 * x
    return simplify_mul(constant_symbol<-1>{}, std::forward<Rhs>(rhs));
  else if constexpr (std::is_arithmetic_v<std::remove_cvref_t<Lhs>> && std::is_arithmetic_v<std::remove_cvref_t<Rhs>>)
    return lhs - rhs;
  // Pattern: A - (A + B) -> -B
  else if constexpr (is_plus_expr_v<Rhs>)
  {
    // Check if the right hand side is a binary sum (A + B) where A matches lhs
    using rhs_t = std::remove_cvref_t<Rhs>;
    constexpr auto tuple_size = std::tuple_size_v<decltype(rhs_t::terms)>;
    if constexpr (tuple_size == 2)
    {
      const auto& t0 = std::get<0>(rhs.terms);
      const auto& t1 = std::get<1>(rhs.terms);
      if constexpr (are_same_symbolic_value_v<Lhs, std::remove_cvref_t<decltype(t0)>>)
      {
         // matches first term: x - (x + y) -> -y
         return simplify_mul(constant_symbol<-1>{}, std::get<1>(rhs.terms));
      }
      else if constexpr (are_same_symbolic_value_v<Lhs, std::remove_cvref_t<decltype(t1)>>)
      {
         // matches second term: x - (y + x) -> -y
         return simplify_mul(constant_symbol<-1>{}, std::get<0>(rhs.terms));
      }
      else
         return simplify_add(std::forward<Lhs>(lhs), simplify_mul(constant_symbol<-1>{}, std::forward<Rhs>(rhs)));
    }
    else
       return simplify_add(std::forward<Lhs>(lhs), simplify_mul(constant_symbol<-1>{}, std::forward<Rhs>(rhs)));
  }
  else
    // Canonicalize: A - B -> A + (-1 * B)
    return simplify_add(std::forward<Lhs>(lhs), simplify_mul(constant_symbol<-1>{}, std::forward<Rhs>(rhs)));
}

// Simplification for multiplication
template<typename Lhs, typename Rhs>
constexpr auto simplify_mul(Lhs&& lhs, Rhs&& rhs)
{
  // Pattern: 0 * x -> 0, x * 0 -> 0
  if constexpr (is_structural_zero_v<Lhs> || is_structural_zero_v<Rhs>)
    return constant_symbol<0>{};
  // Pattern: 1 * x -> x
  else if constexpr (is_structural_one_v<Lhs>)
    return std::forward<Rhs>(rhs);
  // Pattern: x * 1 -> x
  else if constexpr (is_structural_one_v<Rhs>)
    return std::forward<Lhs>(lhs);
  else if constexpr (is_constant_symbol_v<Lhs> && is_constant_symbol_v<Rhs>)
    return constant_symbol<std::remove_cvref_t<Lhs>::value * std::remove_cvref_t<Rhs>::value>{};
  // Flattening Logic
  else if constexpr (is_mul_expr_v<Lhs> && is_mul_expr_v<Rhs>)
    return make_flat_expression<std::multiplies<void>>(std::forward<Lhs>(lhs).terms, std::forward<Rhs>(rhs).terms);
  else if constexpr (is_mul_expr_v<Lhs>)
    // Use merge_flat_expression for append (fallback logic handles it)
    return merge_flat_expression<std::multiplies<void>>(std::forward<Lhs>(lhs).terms, std::forward<Rhs>(rhs));
  else if constexpr (is_mul_expr_v<Rhs>)
    // Use make_flat_expression with temporary tuple for prepend
    return make_flat_expression<std::multiplies<void>>(std::make_tuple(std::forward<Lhs>(lhs)),
                                                       std::forward<Rhs>(rhs).terms);
  // Pattern: x^n * x^m → x^(n+m)
  else if constexpr (is_power_expr_v<Lhs> && is_power_expr_v<Rhs> && are_same_symbolic_value_v<expr_lhs_t<Lhs>, expr_lhs_t<Rhs>>)
  {
      return expr_lhs_t<Lhs>{} ^ simplify_add(get_rhs_val(std::forward<Lhs>(lhs)), get_rhs_val(std::forward<Rhs>(rhs)));
  }
  // Pattern: x * x → x^2
  else if constexpr (are_same_symbolic_value_v<Lhs, Rhs>)
    return std::forward<Lhs>(lhs) ^ constant_symbol<2>{};
  // Pattern: x^n * x → x^(n+1) (where n is a constant_symbol)
  else if constexpr (is_power_expr_v<Lhs> && are_same_symbolic_value_v<expr_lhs_t<Lhs>, Rhs>)
  {
    using exp_t = expr_rhs_t<Lhs>;
    if constexpr (requires { exp_t::value; })
      return expr_lhs_t<Lhs>{} ^ constant_symbol<exp_t::value + 1>{};
    else
      return symbolic_expression<std::multiplies<void>, std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>(
        std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
  }
  // Pattern: x * x^n → x^(n+1) (where n is a constant_symbol)
  else if constexpr (is_power_expr_v<Rhs> && are_same_symbolic_value_v<Lhs, expr_lhs_t<Rhs>>)
  {
    using exp_t = expr_rhs_t<Rhs>;
    if constexpr (requires { exp_t::value; })
      return expr_lhs_t<Rhs>{} ^ constant_symbol<exp_t::value + 1>{};
    else
      return symbolic_expression<std::multiplies<void>, std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>(
        std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
  }
  else if constexpr (std::is_arithmetic_v<std::remove_cvref_t<Lhs>> && std::is_arithmetic_v<std::remove_cvref_t<Rhs>>)
    return lhs * rhs;
  // Pattern: ((... * Z) * ...) / Z → Remove Z deeply
  else if constexpr (is_mul_expr_v<Lhs> && is_deep_cancellation_v<get_lhs_t<Lhs>, Rhs, std::multiplies<void>>)
    return simplify_mul(simplify_div(get_lhs_val(std::forward<Lhs>(lhs)), rhs), get_rhs_val(std::forward<Lhs>(lhs)));
  else
    return symbolic_expression<std::multiplies<void>, std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>(
      std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
}

// Simplification for division
template<typename Lhs, typename Rhs>
constexpr auto simplify_div(Lhs&& lhs, Rhs&& rhs)
{
  if constexpr (is_structural_one_v<Rhs>)
    return std::forward<Lhs>(lhs);
  else if constexpr (are_same_symbolic_value_v<Lhs, Rhs>)
    return constant_symbol<1>{};
  // Pattern: (A * B) / B → A
  else if constexpr (is_mul_expr_v<Lhs> && are_same_symbolic_value_v<expr_rhs_t<Lhs>, Rhs>)
    return expr_lhs_t<Lhs>{};
  // Pattern: (A * B) / A → B
  else if constexpr (is_mul_expr_v<Lhs> && are_same_symbolic_value_v<expr_lhs_t<Lhs>, Rhs>)
    return expr_rhs_t<Lhs>{};
  // Pattern: ((... * Z) * ...) / Z → Remove Z deeply
  else if constexpr (is_mul_expr_v<Lhs> && is_deep_cancellation_v<get_lhs_t<Lhs>, Rhs, std::multiplies<void>>)
    return simplify_mul(simplify_div(get_lhs_val(std::forward<Lhs>(lhs)), rhs), get_rhs_val(std::forward<Lhs>(lhs)));
  else if constexpr (std::is_arithmetic_v<std::remove_cvref_t<Lhs>> && std::is_arithmetic_v<std::remove_cvref_t<Rhs>>)
    return lhs / rhs;
  // Pattern: x^n / x → x^(n-1)
  else if constexpr (is_power_expr_v<Lhs> && are_same_symbolic_value_v<expr_lhs_t<Lhs>, Rhs>)
  {
    using exp_t = expr_rhs_t<Lhs>;
    if constexpr (requires { exp_t::value; })
      return simplify_pow(expr_lhs_t<Lhs>{}, constant_symbol<exp_t::value - 1>{});
    else
      return symbolic_expression<std::divides<void>, std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>(
        std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
  }
  // Pattern: x / x^n → x^(1-n)
  else if constexpr (is_power_expr_v<Rhs> && are_same_symbolic_value_v<Lhs, expr_lhs_t<Rhs>>)
  {
      using exp_t = expr_rhs_t<Rhs>;
      if constexpr (requires { exp_t::value; })
        return simplify_pow(expr_lhs_t<Rhs>{}, constant_symbol<1 - exp_t::value>{});
      else
        return symbolic_expression<std::divides<void>, std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>(
          std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
  }
  // Pattern: x^n / x^m → x^(n-m)
  else if constexpr (is_power_expr_v<Lhs> && is_power_expr_v<Rhs> && are_same_symbolic_value_v<expr_lhs_t<Lhs>, expr_lhs_t<Rhs>>)
  {
      return simplify_pow(expr_lhs_t<Lhs>{}, simplify_sub(get_rhs_val(std::forward<Lhs>(lhs)), get_rhs_val(std::forward<Rhs>(rhs))));
  }
  else
    return symbolic_expression<std::divides<void>, std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>(
      std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
}

// Simplification for power
template<typename Base, typename Exp>
constexpr auto simplify_pow(Base&& base, Exp&& exp)
{
  if constexpr (is_structural_zero_v<Exp>)
    return constant_symbol<1>{};
  else if constexpr (is_structural_one_v<Exp>)
    return std::forward<Base>(base);
  else if constexpr (is_structural_zero_v<Base>)
    return constant_symbol<0>{};
  else if constexpr (is_structural_one_v<Base>)
    return constant_symbol<1>{};
  else if constexpr (std::is_arithmetic_v<std::remove_cvref_t<Base>> && std::is_arithmetic_v<std::remove_cvref_t<Exp>>)
    return std::pow(base, exp);
  // Pattern: (x^n)^m → x^(n*m)
  else if constexpr (is_power_expr_v<Base>)
  {
      return simplify_pow(get_lhs_val(std::forward<Base>(base)), simplify_mul(get_rhs_val(std::forward<Base>(base)), std::forward<Exp>(exp)));
  }
  else
    return symbolic_expression<power<void>, std::remove_cvref_t<Base>, std::remove_cvref_t<Exp>>(
      std::forward<Base>(base), std::forward<Exp>(exp));
}

template<typename T = void>
struct power
{
  template<typename U = T, typename V>
  constexpr auto operator()(U&& Lhs, V&& Rhs) const
  { return simplify_pow(std::forward<U>(Lhs), std::forward<V>(Rhs)); }
};

template<typename Arg>
constexpr auto simplify_neg(Arg&& arg)
{
  if constexpr (is_structural_zero_v<Arg>)
    return constant_symbol<0>{};
  else if constexpr (is_negate_expr_v<Arg>)
    // Pattern: -(-x) -> x (using unary negate)
    return get_lhs_val(std::forward<Arg>(arg));
  else if constexpr (is_minus_expr_v<Arg> && std::is_same_v<expr_lhs_t<std::remove_cvref_t<Arg>>, constant_symbol<0>>)
    // Pattern: -(-x) -> x (where -x is represented as 0 - x)
    return expr_rhs_t<std::remove_cvref_t<Arg>>{}; 
  else if constexpr (std::is_arithmetic_v<std::remove_cvref_t<Arg>>)
    return -arg;
  else
    return symbolic_expression<std::negate<void>, std::remove_cvref_t<Arg>>(std::forward<Arg>(arg));
}

// Main simplification dispatcher
template<typename Operator, typename Lhs, typename Rhs>
constexpr auto simplify_expression(const Operator& op, Lhs&& lhs, Rhs&& rhs)
{
  if constexpr (std::is_same_v<Operator, std::plus<void>>)
    return simplify_add(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
  else if constexpr (std::is_same_v<Operator, std::minus<void>>)
    return simplify_sub(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
  else if constexpr (std::is_same_v<Operator, std::multiplies<void>>)
    return simplify_mul(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
  else if constexpr (std::is_same_v<Operator, std::divides<void>>)
    return simplify_div(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
  else
  {
    // Fallback for unhandled binary operators
    if constexpr (std::is_arithmetic_v<std::remove_cvref_t<Lhs>> && std::is_arithmetic_v<std::remove_cvref_t<Rhs>>)
      return op(std::forward<Lhs>(lhs), std::forward<Rhs>(rhs));
    else
      return symbolic_expression<Operator, std::remove_cvref_t<Lhs>, std::remove_cvref_t<Rhs>>(std::forward<Lhs>(lhs),
                                                                                               std::forward<Rhs>(rhs));
  }
}

// Unary simplification dispatcher
template<typename Operator, typename Arg>
constexpr auto simplify_expression(const Operator& op, Arg&& arg)
{
  if constexpr (std::is_same_v<Operator, std::negate<void>>)
    return simplify_neg(std::forward<Arg>(arg));
  else
  {
    // Fallback for unhandled unary operators (like sin, cos, etc.)
    if constexpr (std::is_arithmetic_v<std::remove_cvref_t<Arg>>)
      return op(std::forward<Arg>(arg));
    else
      return symbolic_expression<Operator, std::remove_cvref_t<Arg>>(std::forward<Arg>(arg));
  }
}



} // end namespace lam::symbols
