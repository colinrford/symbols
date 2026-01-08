/*
 * test_utils.cppm
 * part of test suite for lam.symbols
 * Extending Author: Colin Ford
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info
 */

export module lam.test.utils;

import std;

export namespace lam::test::utils
{

// Helper to check closeness, handling potential symbolic failures
// Uses manual absolute value to avoid std::abs/valarray issues with symbolic types
template<typename T, typename U>
constexpr bool check_close(const T& val, U expected, double tol = 1e-9)
{
  if constexpr (std::is_arithmetic_v<T>)
  {
    auto diff = val - expected;
    return (diff < 0 ? -diff : diff) < tol;
  }
  else
  {
    return false; // Symbolic result implies simplification failed
  }
}

} // namespace lam::test::utils
