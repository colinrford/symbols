# symbols
a perhaps primitive compile-time symbolic computation library. Would be great to extend this further.

Can find what is in here from Vincent Reverdy's CppCon 2023 presentation *Symbolic Calculus for High-performance Computing From Scratch Using C++23*, a pdf of which is contained in this repository. [Here](https://www.youtube.com/watch?v=lPfA4SFojao) is a link to the YouTube video. So far I have personally added very little.

If paired with a library like [gcem](https://gcem.readthedocs.io/en/latest/#) – as far as I know – will allow use for this entirely at compile time, or, at the very least, compile with `constexpr`. [Here](https://godbolt.org/z/Gosx441h3) is a working example linked with gcem. The code here uses the Standard Library, so until C++26 (or later), `constexpr` will not compile with certain expressions; but switching between using `std` math or `gcem` math is a matter of replacing `std::` by `gcem::`, so if you feel like making the switch it's immediate – check out the example on godbolt...

At the end of the slides are some exercises for the reader...

Partial substitution
  - for now only full substitution works
  - partial substitution needs more work: e.g. f(a = 5.0, w = 2.5) should generate a new formula

Rewriting
  - Replacing terms by others (rewriting) need also more work: f(x = y / z) should generate a new formula
  - Simplification based on mathematical concepts
  - Symbolic calculus (derivatives, integrals)
  - Full blown custom rule-based rewriting

Again, would love to grow this seed.
