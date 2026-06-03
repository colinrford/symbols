// SPDX-License-Identifier: CC0-1.0
// SPDX-FileCopyrightText: 2023 Vincent Reverdy
// SPDX-FileCopyrightText: 2025-2026 Colin Ford

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

export module lam.symbols;
export import :traits;
export import :core;
export import :engine;
export import :operators;
// Experimental partitions — held off the public `import lam.symbols;` surface
// until they stabilize (morphisms blocked on C++26 reflection; calculus.diff +
// laws/lawsets still maturing). Build with -DLAM_SYMBOLS_EXPERIMENTAL=ON to
// export + exercise them. The partition sources still compile regardless; this
// only controls whether they reach external consumers.
#ifdef LAM_SYMBOLS_EXPERIMENTAL
export import :laws;
export import :lawsets;
export import :morphisms;
export import :calculus.diff;
#endif
export import :config;

import std;

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
