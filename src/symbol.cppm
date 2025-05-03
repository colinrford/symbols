/*
 * Provided under CC0 1.0 Universal – Public Domain Dedication license
 * Author: Vincent Reverdy (LAPP, France)
 *  Provided in CppCon 2023 Presentation
 *  "Symbolic Calculus for High-performance Computing From Scratch Using C++23"
 *    – symbolic: the concept of every part of a symbolic expression
 *    - symbol: to declare symbolic variable
 *    - symbol id: to uniquely index symbols and make them comparable
 *    - symbol binder: to bind a free symbol to a value
 *    - symbol constraint: a concept wrapper to constrain symbols
 *    - constant symbol: symbolic constants like \pi
 *    - symbolic expression: the symbolic abstract syntax tree built from
 *      operators
 *    - formula: to give name and call symbolic expressions
 *    - substitution: the utility to replace symbols by their values and
 *      compute the result
 *
 * "copy-and-pasted" (mostly) from slides by: Colin Ford
 */

module;

import std;

export module symbols;

export namespace symbols
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

template <typename>
struct symbol_id
{
  static constexpr auto singleton = []{};
  static constexpr const void* address = std::addressof(singleton);
};

template <typename Lhs, typename Rhs>
constexpr std::strong_ordering
operator<=>(symbol_id<Lhs>, symbol_id<Rhs>)
{
  return std::compare_three_way{}(symbol_id<Lhs>::address,
                                  symbol_id<Rhs>::address);
}

// Symbol binder typename definition
template <typename Symbol, typename T>
struct symbol_binder
{
  // Types and constants
  using symbol_type = Symbol;
  using value_type = std::remove_cvref_t<T>;
  static constexpr Symbol symbol = {};
  // Constructors
  template <typename U>
  requires std::is_convertible_v
           <U&&, requalify_as_const_t<remove_rvalue_reference_t<T>>>
  constexpr symbol_binder(Symbol, U&& x) noexcept
  (
    std::is_nothrow_convertible_v
    <
      U&&,
      requalify_as_const_t<remove_rvalue_reference_t<T>>
    >
  ) : value(std::forward<U>(x)) {}
  // Accessors
  constexpr const value_type& operator()() const noexcept { return value; }
  //const value_type& operator()() const noexcept { return value; }
  // Implementation details: data members
  private:
    requalify_as_const_t<remove_rvalue_reference_t<T>> value;
};
// Deduction guide
template <typename Symbol, typename T>
symbol_binder(Symbol, T&&) -> symbol_binder<Symbol, T&&>;

struct unconstrained
{
  template <typename T>
  struct trait : std::true_type {};
};

// The substitution wrapper itself
template <typename... Binders>
struct substitution; // need (this) forward declaration

template <typename Trait = unconstrained,
          auto Id = symbol_id<decltype([]{})>{}>
struct symbol
{
  static constexpr auto id = Id; // unique identifier

  template <typename Arg> // binding mechanism
  requires Trait::template trait<std::remove_cvref_t<Arg>>::value
  constexpr symbol_binder<symbol, Arg&&> operator=(Arg&& arg) const
  { return symbol_binder(*this, std::forward<Arg>(arg)); }

  template<typename... Binders>
  constexpr auto operator()(const substitution<Binders...>& s) const
  { return s[id](); /* <–– everything happens here */ }
};

// singleton and address have to stay public data members?

// END Binding

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

// Specialization for variable symbol
template <typename T, auto Id>
struct is_symbolic<symbol<T, Id>> : std::true_type {};

template <auto Value>
struct constant_symbol
{
  using type = decltype(Value);
  static constexpr type value = Value;

  template<typename... Binders>
  constexpr auto operator()(const substitution<Binders...>& s) const
  { return value; /* <–– everything happens here */ }
};
// Making it symbolic
template <auto Value>
struct is_symbolic<constant_symbol<Value>> : std::true_type {};
// END architecture

/*
 *  Substitution
 */
// forward declaration??
// Index constant type
template <std::size_t I>
struct index_constant : std::integral_constant<std::size_t, I> {};

// Index constant variable template – NOTE: FAILS
//template <std::size_t I>
//inline constexpr index_constant<I> index = {};

// An indexed substitution element
template <std::size_t I, typename B>
struct substitution_element;

// A helper class to build the substitution
template <typename Sequence, typename... T>
struct substitution_base;
/* // some annoying forward declaration requires this to be defined above
        // The substitution wrapper itself
        template <typename... Binders>
        struct substitution;
*/
// Step 1: substitution providing an index sequence to its base
template <typename... Binders>
struct substitution : substitution_base<std::index_sequence_for<Binders...>,
                                        Binders...>
{
  using base = substitution_base<std::index_sequence_for<Binders...>,
                                 Binders...>;
  using base::base;
  using base::operator[];
};
// deduction guide
template <typename... Binders>
substitution(const Binders&...) -> substitution<Binders...>;

// Step 2: substitution base
template <std::size_t... Index, typename... Binders>
struct substitution_base<std::index_sequence<Index...>, Binders...>
        : substitution_element<Index, Binders>...
{
  using index_sequence = std::index_sequence<Index...>;
  using substitution_element<Index, Binders>::operator[]...;
  constexpr substitution_base(const Binders&... x)
              : substitution_element<Index, Binders>(x)...
  {}
};
// Step 3: substitution element
template <std::size_t I, typename Binder>
struct substitution_element
{
  // Types and constants
  using index = index_constant<I>;
  using id_type = decltype(Binder::symbol_type::id);

  constexpr substitution_element(const Binder& b)
              : bbinder(b)
  {}

  constexpr const Binder& operator[](index) const { return bbinder; }
  constexpr const Binder& operator[](id_type) const { return bbinder; }

  private:
    const Binder bbinder;
};
// END substitution

template <symbolic Expression>
struct formula
{
  using expression = Expression;

  constexpr formula(Expression expr) noexcept {};
  // Call operator where substitution happens
  template <class... Args>
  constexpr auto operator()(Args... args) const noexcept
  { return expression{}(substitution(args...)); }
};

/*
 *  Construction
 */

// The class for symbolic expressions
template <typename Operator, symbolic... Terms>
struct symbolic_expression
{
  template <class... Binders>
  constexpr auto operator()(const substitution<Binders...>& s) const noexcept
  { return Operator{}(Terms{}(s)...); /* <− Everything happens here */ }
};

// Making it symbolic
template <typename Operator, symbolic... Terms>
struct is_symbolic<symbolic_expression<Operator, Terms...>>
        : std::true_type {};

// some operators
template <symbolic Lhs, symbolic Rhs>
constexpr symbolic_expression<std::plus<void>, Lhs, Rhs>
operator+(Lhs, Rhs) noexcept { return {}; }
template <symbolic Lhs, symbolic Rhs>
constexpr symbolic_expression<std::minus<void>, Lhs, Rhs>
operator-(Lhs, Rhs) noexcept { return {}; }
template <symbolic Lhs, symbolic Rhs>
constexpr symbolic_expression<std::multiplies<void>, Lhs, Rhs>
operator*(Lhs, Rhs) noexcept { return {}; }
template <symbolic Lhs, symbolic Rhs>
constexpr symbolic_expression<std::divides<void>, Lhs, Rhs> operator/(Lhs, Rhs)
noexcept { return {}; }
template <symbolic arg>
constexpr symbolic_expression<std::negate<void>, arg> operator-(arg)
noexcept { return {}; }

/*template <typename T = void>
struct power
{
  template <typename U = T, typename V>
  constexpr auto operator()(U&& Lhs, V&& Rhs) const
    -> decltype(std::pow(std::forward<U>(Lhs), std::forward<V>(Rhs)))
  { return std::pow(std::forward<U>(Lhs), std::forward<V>(Rhs)); }

};
template <symbolic Lhs, symbolic Rhs>
constexpr symbolic_expression<power<void>, Lhs, Rhs> operator^(Lhs, Rhs)
            noexcept { return {}; }*/
// end operators

} // end namespace symbols



// exercises for the reader...
//  Partial substitution
//    – for now only full substitution works
//    – partial substitution needs more work: e.g. f(a = 5.0, w = 2.5) should
//      generate a new formula
//  Rewriting
//    – Replacing terms by others (rewriting) need also more work: f(x = y / z)
//      should generate a new formula
//    – Simplification based on mathematical concepts
//    – Symbolic calculus (derivatives, integrals)
//    – Full blown custom rule-based rewriting
