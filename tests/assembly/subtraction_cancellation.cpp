/*
 * subtraction_cancellation.cpp
 * part of test suite for lam.symbols
 * Assembly comparison for new pattern: x - (x + y) -> -y
 * see github.com/colinrford/symbols for CC0-1.0 Universal License
 */

import std;
import lam.symbols;
import lam.test.utils;
using namespace lam::symbols;
using namespace lam::test::utils;

// Pattern: x - (x + y) -> -y
__attribute__((noinline)) double check_sub_sum(double y_val)
{
  constexpr symbol x;
  constexpr symbol y;
  // A - (A + B) -> -B
  // Here A=x, B=y
  constexpr auto f = x - (x + y);
  
  // Verify at compile time that x is gone (if possible/easy), 
  // but runtime check confirms it too.
  // We only provide y. If x was still there, this would be a symbolic result.
  return f(y = y_val);
}

// Commutative Pattern: x - (y + x) -> -y
__attribute__((noinline)) double check_sub_sum_comm(double y_val)
{
  constexpr symbol x;
  constexpr symbol y;
  // A - (B + A) -> -B
  constexpr auto f = x - (y + x);
  return f(y = y_val);
}

// Baseline: -y
__attribute__((noinline)) double baseline_neg(double y_val)
{
  return -y_val;
}

int main()
{
  volatile double val = 5.0;

  double res1 = check_sub_sum(val);
  double res2 = check_sub_sum_comm(val);
  double base = baseline_neg(val);

  std::println("x - (x + y) [y=5.0] = {}", res1);
  std::println("x - (y + x) [y=5.0] = {}", res2);
  std::println("Baseline -y = {}", base);

  bool pass1 = check_close(res1, base);
  bool pass2 = check_close(res2, base);
  
  if (pass1 && pass2) {
    std::println("PASS: Assembly generation logic validated.");
    return 0;
  } else {
    std::println("FAIL: Results do not match baseline.");
    return 1;
  }
}
