
import std;
import symbols;
using namespace symbols;

template <typename T>
using std_vector = std::vector<T>; // personal preference

struct sin_symbol
{
  template <typename Arg>
  constexpr auto operator()(Arg&& arg)
  { return std::sin(std::forward<Arg>(arg)); }
};

// function builder
template <symbolic Arg>
constexpr symbolic_expression<sin_symbol, Arg> sin(Arg) noexcept
{ return {}; }

struct exp_symbol
{
  template <typename Arg>
  constexpr auto operator()(Arg&& arg)
  { return std::exp(std::forward<Arg>(arg)); }
};

// function builder
template <symbolic Arg>
constexpr symbolic_expression<exp_symbol, Arg> exp(Arg) noexcept
{ return {}; }

template <typename T = void>
struct power
{
  template <typename U = T, typename V>
  constexpr auto operator()(U&& Lhs, V&& Rhs) const
    -> decltype(std::pow(std::forward<U>(Lhs), std::forward<V>(Rhs)))
  { return std::pow(std::forward<U>(Lhs), std::forward<V>(Rhs)); }

};
template <symbolic Lhs, symbolic Rhs>
constexpr symbolic_expression<power<void>, Lhs, Rhs> operator^(Lhs, Rhs) noexcept { return {}; }

int main()
{
  constexpr symbol a;
  constexpr constant_symbol<2> two;
  constexpr constant_symbol<3> three;
  constexpr symbol omega;
  constexpr symbol t;
  constexpr symbol phi;

  constexpr formula f = (a ^ two) * sin(omega * t + phi);

  double y = f(a = std::numbers::pi_v<double>, omega = 2.5, t = 1.6, phi = 0);

  std::println("y = {0}", y);

  constexpr symbol b;
  constexpr formula g = a * exp(-(b ^ two) / two);
  double z = g(a = 3, b = 2.);
  std::println("z = {0}", z);

  std::println("y + z = {0}", y + z);

}
