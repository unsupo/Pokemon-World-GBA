#pragma once
// Shim for toolchains without newlib (e.g. Homebrew arm-none-eabi-gcc).
// Only declares the functions actually used in this codebase.
#include <stdarg.h>
#include <stddef.h>

int snprintf(char *s, size_t n, const char *fmt, ...);
int sprintf(char *s, const char *fmt, ...);
int vsnprintf(char *s, size_t n, const char *fmt, va_list ap);
int vsprintf(char *s, const char *fmt, va_list ap);
