// SPDX-License-Identifier: BSD-2-Clause
/*
 * File: include/debug.h
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Description:
 * Various debug functions.
 */

#pragma once

#include <types.h>

#define assert(cond, msg) debug_assert((cond), (msg), __func__, __FILE__, __LINE__)
#define assert_warn(cond, msg) debug_assert_warn((cond), (msg), __func__, __FILE__, __LINE__)

char debug_putchar(char chr);
int debug_print(const char* str);
int debug_printf(const char* fmt, ...);
int debug_vprintf(const char* fmt, va_list args);
void debug_assert(int cond, const char* message, const char* func, const char* file, int line);
void debug_assert_warn(int cond, const char* message, const char* func, const char* file, int line);
