/**
 * @file debug.h
 * @author Popa Vlad (Garnek0)
 * @copyright BSD-2-Clause
 * 
 * @brief
 * Useful debug functions.
 */

#pragma once

#include <symphony/types.h>

/**@{*/
/** @brief Loglevel for debug_log(). */
#define LOGLEVEL_TRACE 1
#define LOGLEVEL_DEBUG 2
#define LOGLEVEL_INFO 3
#define LOGLEVEL_WARN 4
#define LOGLEVEL_ERROR 5
#define LOGLEVEL_FATAL 6
/**@}*/

/**
 * @brief Print an error string and panic if `cond` evaluates to false.
 *
 * @param cond The condition to evaluate
 * @param msg Error string to be printed if cond == false
 */
#define assert(cond, msg) __debug_assert((cond), (msg), __func__, __FILE__, __LINE__)

/**
 * @brief Print a warning if `cond` evaluates to false. Unlike assert(), assert_warn()
 * does not panic if the assertion is false.
 *
 * @param cond The condition to evaluate
 * @param msg Error string to be printed if cond == false
 */
#define assert_warn(cond, msg) __debug_assert_warn((cond), (msg), __func__, __FILE__, __LINE__)

/**
 * @brief Internal assert() function. Should not be used.
 */
void __debug_assert(int cond, const char* message, const char* func, const char* file, int line);

/**
 * @brief Internal assert_warn() function. Should not be used.
 */
void __debug_assert_warn(int cond, const char* message, const char* func, const char* file, int line);

/**
 * @brief Print a character on all appropriate output devices.
 *
 * @param chr The character to be printed
 *
 * @return The printed character
 */
char debug_putchar(char chr);

/**
 * @brief Print a string on all appropriate output devices.
 *
 * @param str The string to be printed
 *
 * @return The number of printed characters.
 */
int debug_print(const char* str);

/**
 * @brief Print a string on all appropriate output devices, with formatting support.
 *
 * @param fmt The format string.
 * @param ... Sequence of values to be used to replace the format specifiers in fmt
 *
 * @return The number of printed characters.
 */
int debug_printf(const char* fmt, ...);

/**
 * @brief Print a string on all appropriate output devices, with formatting support
 * via variadic list.
 *
 * @param fmt The format string.
 * @param args Variadic list of values to be used to replace the format specifiers in fmt
 *
 * @return The number of printed characters.
 */
int debug_vprintf(const char* fmt, va_list args);

/**
 * @brief Print a kernel log on all appropriate output devices, with formatting support.
 *
 * @param loglevel The log's loglevel. Must be a valid LOGLEVEL_* value. 
 * @param fmt The format string.
 * @param ... Sequence of values to be used to replace the format specifiers in fmt
 *
 * @return The number of printed characters.
 */
int debug_log(int loglevel, const char* fmt, ...);

/**
 * @brief Trigger a kernel panic.
 *
 * @details This function prints a formatted error message and halts the system. 
 * At this point, the user must manually restart their computer.
 *
 * @param fmt Error message
 * @param ... Sequence of values to be used to replace the format specifiers in fmt
 */
void debug_panic(const char* fmt, ...);
