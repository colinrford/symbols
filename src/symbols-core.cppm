/*
 * Provided under CC0 1.0 Universal – Public Domain Dedication license
 * Original Author: Vincent Reverdy (LAPP, France)
 * Source: CppCon 2023 "Symbolic Calculus for High-performance Computing From Scratch Using C++23"
 */

/*
 * lam.symbols:core
 * Description: The atomic units of the symbolic language.
 * Content: symbol, constant_symbol, symbol_id, symbol_binder, substitution.
 * Note: Contains cyclic dependencies resolved via recursive templates.
 * Extending Author: Colin Ford
 */

module;

import std;

export module lam.symbols:core;
import :traits;

export namespace lam::symbols
{

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
  constexpr symbol_binder<symbol, Arg&&> operator=(Arg&& arg) const // NOLINT(cppcoreguidelines-c-copy-assignment-signature)
  { return symbol_binder(*this, std::forward<Arg>(arg)); }

  template<typename... Binders>
  constexpr auto operator()(const substitution<Binders...>& s) const
  {
    // Find the LAST binder that matches this symbol (to allow overriding)
    constexpr auto match_idx = []<std::size_t... Is>(std::index_sequence<Is...>) {
      std::size_t found = sizeof...(Binders); 
      ((std::is_same_v<typename std::remove_reference_t<Binders>::symbol_type, symbol> ? (found = Is) : 0), ...);
      return found;
    }(std::index_sequence_for<Binders...>{});

    if constexpr (match_idx < sizeof...(Binders)) 
    { // Use index-based access which is unambiguous
      return s[index_constant<match_idx>{} /*index*/](); 
    } else 
    {
      return *this;
    }
  }

  // Convenience operator for direct substitution
  template <class... Args>
  constexpr auto operator()(Args... args) const noexcept
  { return (*this)(substitution(args...)); }

  friend constexpr bool operator==(const symbol&, const symbol&) { return true; }
  friend constexpr bool operator!=(const symbol&, const symbol&) { return false; }
};

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

  // Convenience operator for direct substitution (e.g. 0(x=1)) usually typical for generic code
  template <class... Args>
  constexpr auto operator()(Args... args) const noexcept
  { 
    return value; 
    // Note: we just ignore args because it's a constant. 
    // No need to build substitution object.
  }

  friend constexpr bool operator==(const constant_symbol&, const constant_symbol&) { return true; }
  friend constexpr bool operator!=(const constant_symbol&, const constant_symbol&) { return false; }
};
// Making it symbolic
template <auto Value>
struct is_symbolic<constant_symbol<Value>> : std::true_type {};

/*
 *  Substitution
 */

// An indexed substitution element
template <std::size_t I, typename B>
struct substitution_element;

// A helper class to build the substitution
template <typename Sequence, typename... T>
struct substitution_base;

// Step 1: substitution providing an index sequence to its base
template <typename... Binders>
struct substitution : substitution_base<std::index_sequence_for<Binders...>,
                                        Binders...>
{
  using base = substitution_base<std::index_sequence_for<Binders...>,
                                 Binders...>;
  using base::base;
  using base::operator[];
  
  // Helper to check if a symbol ID is bound
  template <auto Id>
  static constexpr bool contains() {
    return (std::is_same_v<decltype(std::remove_reference_t<Binders>::symbol_type::id), decltype(Id)> || ...);
  }
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
  const Binder bbinder; // NOLINT(cppcoreguidelines-avoid-const-or-ref-data-members)
};
// END substitution

} // end namespace lam::symbols
