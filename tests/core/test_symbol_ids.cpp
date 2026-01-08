/*
 * test_symbol_ids.cpp
 * part of test suite for lam.symbols
 * Extending Author: Colin Ford
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info
 */

import std;
import lam.symbols;
using namespace lam::symbols;

// Test to verify that different symbol instances are correctly distinguished
// This tests the symbol ID comparison logic

int main()
{
  // Create multiple distinct symbols
  constexpr symbol a1;
  constexpr symbol a2; // Different instance, different ID
  constexpr symbol b;

  // Test 1: Each symbol should be distinct
  auto expr1 = a1 + b;
  auto expr2 = a2 + b;

  // Binding a1 should not affect a2
  auto partial1 = expr1(a1 = 5.0);
  auto partial2 = expr2(a2 = 10.0);

  auto result1 = partial1(b = 2.0);
  auto result2 = partial2(b = 2.0);

  std::println("Test 1 - Distinct symbols:");
  std::println("  expr1(a1=5, b=2) = {}", result1);  // Should be 7
  std::println("  expr2(a2=10, b=2) = {}", result2); // Should be 12

  bool test1_ok = (std::abs(result1 - 7.0) < 1e-9) && (std::abs(result2 - 12.0) < 1e-9);

  // Test 2: Same symbol used in different expressions
  constexpr symbol x;
  constexpr symbol y;
  auto expr3 = x + y;
  auto expr4 = x * y;

  // Partial substitution on one should not affect the other
  auto partial3 = expr3(x = 3.0);
  auto partial4 = expr4(x = 4.0);

  auto result3 = partial3(y = 2.0);
  auto result4 = partial4(y = 2.0);

  std::println("\nTest 2 - Same symbol in different expressions:");
  std::println("  (x+y) with x=3, y=2 = {}", result3); // Should be 5
  std::println("  (x*y) with x=4, y=2 = {}", result4); // Should be 8

  bool test2_ok = (std::abs(result3 - 5.0) < 1e-9) && (std::abs(result4 - 8.0) < 1e-9);

  // Test 3: Multiple symbols with same name pattern (but different IDs)
  constexpr symbol var1;
  constexpr symbol var2;
  constexpr symbol var3;
  auto complex = var1 + var2 * var3;

  // Partial substitution
  auto step1 = complex(var1 = 1.0);
  auto step2 = step1(var2 = 2.0);
  auto result5 = step2(var3 = 3.0);

  std::println("\nTest 3 - Multiple distinct symbols:");
  std::println("  var1 + var2 * var3 with (1, 2, 3) = {}", result5); // Should be 7

  bool test3_ok = std::abs(result5 - 7.0) < 1e-9;

  // Test 4: Verify symbol IDs are actually different
  // This is a compile-time check - if IDs were the same, we'd get wrong results
  constexpr symbol s1;
  constexpr symbol s2;
  auto test_expr = s1 + s2;

  // If s1 and s2 had the same ID, binding s1 would also bind s2
  auto partial_test = test_expr(s1 = 100.0);
  auto final_result = partial_test(s2 = 1.0);

  std::println("\nTest 4 - Symbol ID uniqueness:");
  std::println("  s1 + s2 with s1=100, s2=1 = {}", final_result); // Should be 101

  bool test4_ok = std::abs(final_result - 101.0) < 1e-9;

  // Summary
  bool all_ok = test1_ok && test2_ok && test3_ok && test4_ok;

  std::println("\n=== Summary ===");
  std::println("Test 1 (Distinct symbols): {}", test1_ok ? "PASS" : "FAIL");
  std::println("Test 2 (Same symbol, different expressions): {}", test2_ok ? "PASS" : "FAIL");
  std::println("Test 3 (Multiple symbols): {}", test3_ok ? "PASS" : "FAIL");
  std::println("Test 4 (ID uniqueness): {}", test4_ok ? "PASS" : "FAIL");
  std::println("\nOverall: {}", all_ok ? "ALL TESTS PASSED" : "SOME TESTS FAILED");

  return all_ok ? 0 : 1;
}
