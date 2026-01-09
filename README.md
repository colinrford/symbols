# `lam.symbols`

## background
What is in here is an outgrowth of Vincent Reverdy's CppCon 2023 presentation *Symbolic Calculus for High-performance Computing From Scratch Using C++23*, a pdf of which is contained in this repository. [Here](https://www.youtube.com/watch?v=lPfA4SFojao) is a link to the YouTube video.

If paired with a library like [gcem](https://gcem.readthedocs.io/en/latest/#) will allow use for this entirely at compile time, that is, compile within `constexpr` and `consteval` contexts. [Here](https://godbolt.org/z/Gosx441h3) is a working example linked with gcem. The code here uses the Standard Library, so until C++26 (or later), `constexpr` will not compile with certain expressions; but switching between using `std` math or `gcem` math is a matter of replacing `std::` by `gcem::`, so if you feel like making the switch it's immediate – check out the example on godbolt...

## symbols, a [LAM](https://github.com/colinrford/lam) library
`lam.symbols` is a c++ module and is a part of [LAM](https://github.com/colinrford/lam). It has been extended from it's original state to implement some of the exercises for the reader left at the end of the original presentation.

## building
You will need `cmake` `3.31.6` or later, `ninja`, and a compiler that `cmake` supports with `import std;`. `lam.symbols` depends on the c++ standard library.

First of all grab a copy:
```
git clone www.github.com/colinrford/symbols
cd symbols
```
then, in the directory `symbols`
```
mkdir build && cd build
cmake .. -G Ninja
```
and so long as this succeeds, run
```
ninja
```

Your Cmake mileage may vary, on `macOS` with homebrew-installed `clang++`, `cmake` may not find and link everything properly, and I have had to add more flags in the build commands. You may be able to get away with a simple `cmake ..` and `ninja`.

## usage
```cpp
import lam.symbols;
using namespace lam::symbols;

constexpr symbol x, y;
constexpr auto expr = (x + y) - y;  // Simplifies to just x at compile time

static_assert(expr(x = 5, y = 10) == 5);  // Verified at compile time!
```

## examples
For now there is but one example included. It computes at compile time the trajectory of Io about Jupiter. An associated Python script will call the `c++` program - the trajectory is already computed - which will print 500 `svg`s; the script converts them with `librsvg` or `inkscape` for feeding into `ffmpeg`, which will stitch the `svg`s together for a short `mp4` video.

The example will not build if it cannot find `librsvg`, `inkscape`, or `ffmpeg`. 
```bash
ninja orbital_mechanics
cd examples/orbital-motion
python make_video.py
```

More interesting examples on the way.
