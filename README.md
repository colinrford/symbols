# symbols
a perhaps primitive compile-time symbolic computation library. Would be great to extend this further.

Can find what is in here from Vincent Reverdy's CppCon 2023 presentation *Symbolic Calculus for High-performance Computing From Scratch Using C++23*, a pdf of which is contained in this repository. [Here](https://www.youtube.com/watch?v=lPfA4SFojao) is a link to the YouTube video. So far I have personally added very little, despite it being quite extensible from the start.

If paired with a library like [gcem](https://gcem.readthedocs.io/en/latest/#) will allow use for this entirely at compile time, that is, compile within `constexpr` and `consteval` contexts. [Here](https://godbolt.org/z/Gosx441h3) is a working example linked with gcem. The code here uses the Standard Library, so until C++26 (or later), `constexpr` will not compile with certain expressions; but switching between using `std` math or `gcem` math is a matter of replacing `std::` by `gcem::`, so if you feel like making the switch it's immediate – check out the example on godbolt...

At the end of the slides are some exercises for the reader...

Partial substitution
  - for now only full substitution works
  - partial substitution needs more work: e.g. `f(a = 5.0, w = 2.5)` should generate a new formula

Rewriting
  - Replacing terms by others (rewriting) need also more work: `f(x = y / z)` should generate a new formula
  - Simplification based on mathematical concepts
  - Symbolic calculus (derivatives, integrals)
  - Full blown custom rule-based rewriting

Again, would love to grow this seed.

## building
Really this is a matter of compiling with the `-std=c++23` flag, but since `import std;` finally works in Clang 18 and GCC 15, and with (experimental) cmake support, symbols now comes as a module and builds with module `std`. It's less steps to just compile it on the command line but apparently there are other preferences out there.

First of all grab a copy:
```
git clone www.github.com/colinrford/symbols
cd symbols
```
then, in the directory `symbols`
```
mkdir build && cd build
cmake .. -G Ninja -CMAKE_CXX_COMPILER=/path/to/your/modern/compiler
```
and so long as this succeeds, run
```
ninja
```
the only file that is created at this time (May 2, 2025) is the `test_symbol` program in `\tests\` directory. `test_symbol` does not do much besides compute a basic expression.

Please excuse any CMake shortcomings, I will try to catch any and fix accordingly.

## examples
More interesting examples on the way. 
