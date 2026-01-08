/*
 * Provided under CC0 1.0 Universal – Public Domain Dedication license
 * Original Author: Vincent Reverdy (LAPP, France)
 * Source: CppCon 2023 "Symbolic Calculus for High-performance Computing From Scratch Using C++23"
 */

/*
 * lam.symbols
 * Extending Author: Colin Ford
 */

module;

import std;

export module lam.symbols;
export import :traits;
export import :core;
export import :engine;
export import :operators;
export import :config;

// exercises for the reader...
//  Partial substitution
//    ✓ IMPLEMENTED: Partial substitution works - f(a = 5.0) returns a new symbolic expression
//  Rewriting
//    ✓ IMPLEMENTED: Symbol rewriting works - f(x = y / z) generates a new formula
//  Simplification
//    ✓ IMPLEMENTED: Basic simplification rules:
//      - x + 0 -> x, 0 + x -> x
//      - x - 0 -> x, x - x -> 0
//      - x * 1 -> x, 1 * x -> x, x * 0 -> 0, 0 * x -> 0
//      - x / 1 -> x, x / x -> 1 (when x != 0)
//  Future enhancements:
//    – More advanced simplification (factorization, common subexpression elimination)
//    – Symbolic calculus (derivatives, integrals)
//    – Full blown custom rule-based rewriting
