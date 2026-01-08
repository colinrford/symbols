/*
 * test_comprehensive.cpp
 * part of test suite for lam.symbols
 * Extending Author: Colin Ford
 * see github.com/colinrford/symbols for CC0-1.0 Universal License, and
 *                                   for more info
 */

import std;
import lam.symbols;
import lam.test.utils;

using namespace lam::symbols;
using namespace lam::test::utils;

// Test framework helpers
struct test_result
{
  bool passed;
  const char* name;
  const char* message;
};

template<typename F>
constexpr test_result run_test(const char* name, F&& test_func)
{
  try
  {
    bool result = std::forward<F>(test_func)();
    return {result, name, result ? "PASSED" : "FAILED"};
  }
  catch (const std::exception& e)
  {
    return {false, name, e.what()};
  }
  catch (...)
  {
    return {false, name, "Unknown exception"};
  }
}

// Test 1: Basic partial substitution
bool test_partial_basic()
{
  constexpr symbol a;
  constexpr symbol b;
  auto expr = a + b;
  auto partial = expr(a = 5.0);
  auto result = partial(b = 2.0);
  return check_close(result, 7.0);
}

// Test 2: Partial substitution returns symbolic expression
bool test_partial_returns_symbolic()
{
  constexpr symbol a;
  constexpr symbol b;
  auto expr = a * b;
  auto partial = expr(a = 3.0);
  // partial should still be symbolic (b is unbound)
  // We can verify by trying to use it in another expression
  constexpr symbol c;
  auto combined = partial + c;
  auto result = combined(b = 2.0, c = 1.0);
  return check_close(result, 7.0); // 3*2 + 1 = 7
}

// Test 3: Chained partial substitutions
bool test_chained_partial()
{
  constexpr symbol a;
  constexpr symbol b;
  constexpr symbol c;
  constexpr symbol d;
  auto expr = a + b + c + d;
  auto step1 = expr(a = 1.0);
  auto step2 = step1(b = 2.0);
  auto step3 = step2(c = 3.0);
  auto result = step3(d = 4.0);
  return check_close(result, 10.0);
}

// Test 4: Rewriting with simple substitution
bool test_rewriting_simple()
{
  constexpr symbol x;
  constexpr symbol y;
  auto f = x + 2.0;
  auto g = f(x = y);
  auto result = g(y = 3.0);
  return check_close(result, 5.0); // 3 + 2 = 5
}

// Test 5: Rewriting with complex expression
bool test_rewriting_complex()
{
  constexpr symbol x;
  constexpr symbol y;
  constexpr symbol z;
  auto f = x * x + x;
  auto g = f(x = y + z);
  auto result = g(y = 2.0, z = 3.0);
  return check_close(result, 30.0); // (2+3)^2 + (2+3) = 25 + 5 = 30
}

// Test 6: Mixed rewriting and substitution
bool test_mixed_rewrite_substitute()
{
  constexpr symbol x;
  constexpr symbol y;
  constexpr symbol z;
  auto expr = x + y + z;
  // Rewrite x = y*2, substitute z = 5.0
  auto partial = expr(x = y * 2.0, z = 5.0);
  auto result = partial(y = 3.0);
  return check_close(result, 14.0); // 3*2 + 3 + 5 = 14
}

// Test 7: Nested rewriting
bool test_nested_rewriting()
{
  constexpr symbol x;
  constexpr symbol y;
  constexpr symbol z;
  constexpr symbol w;
  auto f = x + y;
  auto g = f(x = z * w);
  auto h = g(z = w + 1.0);
  auto result = h(y = 2.0, w = 3.0);
  return check_close(result, 14.0); // (3+1)*3 + 2 = 12 + 2 = 14
}

// Test 8: Arithmetic properties - Commutativity
bool test_commutativity()
{
  constexpr symbol a;
  constexpr symbol b;
  auto expr1 = a + b;
  auto expr2 = b + a;
  auto result1 = expr1(a = 5.0, b = 3.0);
  auto result2 = expr2(a = 5.0, b = 3.0);
  return check_close(result1, result2);
}

// Test 9: Arithmetic properties - Associativity
bool test_associativity()
{
  constexpr symbol a;
  constexpr symbol b;
  constexpr symbol c;
  auto expr1 = (a + b) + c;
  auto expr2 = a + (b + c);
  auto result1 = expr1(a = 1.0, b = 2.0, c = 3.0);
  auto result2 = expr2(a = 1.0, b = 2.0, c = 3.0);
  return check_close(result1, result2);
}

// Test 10: Distributivity
bool test_distributivity()
{
  constexpr symbol a;
  constexpr symbol b;
  constexpr symbol c;
  auto expr1 = a * (b + c);
  auto expr2 = a * b + a * c;
  auto result1 = expr1(a = 2.0, b = 3.0, c = 4.0);
  auto result2 = expr2(a = 2.0, b = 3.0, c = 4.0);
  return check_close(result1, result2);
}

// Test 11: Power operations
bool test_power_operations()
{
  constexpr symbol a;
  constexpr symbol b;
  constexpr constant_symbol<2> two;
  auto expr = a ^ two;
  auto result = expr(a = 3.0);
  return check_close(result, 9.0);
}

// Test 12: Multiple operations in one expression
bool test_complex_expression()
{
  constexpr symbol a;
  constexpr symbol b;
  constexpr symbol c;
  auto expr = (a + b) * c - a;
  auto result = expr(a = 1.0, b = 2.0, c = 3.0);
  return check_close(result, 8.0); // (1+2)*3 - 1 = 9 - 1 = 8
}

// Test 13: Unary negation
bool test_negation()
{
  constexpr symbol a;
  auto expr = -a;
  auto result = expr(a = 5.0);
  return check_close(result, -5.0);
}

// Test 14: Division
bool test_division()
{
  constexpr symbol a;
  constexpr symbol b;
  auto expr = a / b;
  auto result = expr(a = 10.0, b = 2.0);
  return check_close(result, 5.0);
}

// Test 15: Constant symbols
bool test_constant_symbols()
{
  constexpr constant_symbol<5> five;
  constexpr symbol a;
  auto expr = a + five;
  auto result = expr(a = 3.0);
  return check_close(result, 8.0);
}

// Test 16: Overwriting bindings (last one wins)
bool test_binding_override()
{
  constexpr symbol a;
  auto expr = a;
  // Bind a twice - last should win
  auto result = expr(a = 1.0, a = 2.0);
  return check_close(result, 2.0);
}

// Test 17: Zero-argument expression (just a constant)
bool test_constant_expression()
{
  constexpr constant_symbol<42> answer;
  auto expr = answer;
  // Should work without any substitution
  auto result = expr();
  return check_close(result, 42.0);
}

// Test 18: Single symbol expression
bool test_single_symbol()
{
  constexpr symbol x;
  auto expr = x;
  auto result = expr(x = 7.0);
  return check_close(result, 7.0);
}

// Test 19: Deep nesting
bool test_deep_nesting()
{
  constexpr symbol a;
  constexpr symbol b;
  constexpr symbol c;
  constexpr symbol d;
  constexpr symbol e;
  auto expr = (((a + b) * c) - d) / e;
  auto result = expr(a = 1.0, b = 2.0, c = 3.0, d = 4.0, e = 5.0);
  // ((1+2)*3 - 4) / 5 = (9 - 4) / 5 = 1
  return check_close(result, 1.0);
}

// Test 20: Rewriting with power
bool test_rewriting_power()
{
  constexpr symbol x;
  constexpr symbol y;
  auto f = x ^ 2.0;
  auto g = f(x = y + 1.0);
  auto result = g(y = 2.0);
  return check_close(result, 9.0); // (2+1)^2 = 9
}

int main()
{
  try
  {
    std::vector<test_result> results;

    results.push_back(run_test("Partial substitution - basic", test_partial_basic));
    results.push_back(run_test("Partial substitution - returns symbolic", test_partial_returns_symbolic));
    results.push_back(run_test("Chained partial substitutions", test_chained_partial));
    results.push_back(run_test("Rewriting - simple", test_rewriting_simple));
    results.push_back(run_test("Rewriting - complex", test_rewriting_complex));
    results.push_back(run_test("Mixed rewrite and substitute", test_mixed_rewrite_substitute));
    results.push_back(run_test("Nested rewriting", test_nested_rewriting));
    results.push_back(run_test("Commutativity", test_commutativity));
    results.push_back(run_test("Associativity", test_associativity));
    results.push_back(run_test("Distributivity", test_distributivity));
    results.push_back(run_test("Power operations", test_power_operations));
    results.push_back(run_test("Complex expression", test_complex_expression));
    results.push_back(run_test("Negation", test_negation));
    results.push_back(run_test("Division", test_division));
    results.push_back(run_test("Constant symbols", test_constant_symbols));
    results.push_back(run_test("Binding override", test_binding_override));
    results.push_back(run_test("Constant expression", test_constant_expression));
    results.push_back(run_test("Single symbol", test_single_symbol));
    results.push_back(run_test("Deep nesting", test_deep_nesting));
    results.push_back(run_test("Rewriting with power", test_rewriting_power));

    int passed = 0;
    int failed = 0;

    for (const auto& result : results)
    {
      if (result.passed)
      {
        std::println("✓ {}: {}", result.name, result.message);
        passed++;
      }
      else
      {
        std::println("✗ {}: {} - {}", result.name, result.message, result.message);
        failed++;
      }
    }

    std::println("\nSummary: {} passed, {} failed out of {} tests", passed, failed, results.size());

    return failed == 0 ? 0 : 1;
  }
  catch (const std::exception& e)
  {
    std::println(std::cerr, "Unhandled exception in main: {}", e.what());
    return 1;
  }
  catch (...)
  {
    std::println(std::cerr, "Unknown exception in main");
    return 1;
  }
}
