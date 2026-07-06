#pragma once
// Shim for toolchains without newlib (e.g. Homebrew arm-none-eabi-gcc).
// alloca is a GCC builtin — no library needed.
#ifndef alloca
#define alloca(size) __builtin_alloca(size)
#endif
