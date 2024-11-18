/**
 * @file string.h
 * @author Popa Vlad (Garnek0)
 * @copyright BSD-2-Clause
 *
 * @brief
 * libc-like string and memory functions.
 */

#pragma once

#include <symphony/types.h>

/**
 * @brief Copy a chunk of memory.
 *
 * @param dest Destination
 * @param src Source
 * @param n Chunk size in bytes
 */
void *memcpy(void *dest, const void *src, size_t n);

/**
 * @brief Set all bytes in a chunk of memory to a specific value.
 *
 * @param s Source
 * @param c Value
 * @param n Chunk size in bytes
 */
void *memset(void *s, int c, size_t n);

/**
 * @brief Move a chunk of memory.
 *
 * @param dest Destination
 * @param src Source
 * @param n Chunk size in bytes
 */
void *memmove(void *dest, const void *src, size_t n);

/**
 * @brief Compare two chunks of memory.
 *
 * @param s1 First chunk
 * @param s2 Second chunk
 * @param n Chunk size in bytes
 *
 * @return An integer greater than, equal to or less than 0, if the chunk
 * pointed to by s1 is greater than, equal to or less than the chunk pointed 
 * to by s2, respectively.
 */
int memcmp(const void *s1, const void *s2, size_t n);

/**
 * @brief Compare two strings.
 *
 * @param s1 First string
 * @param s2 Second string
 *
 * @return An integer greater than, equal to or less than 0, if the string
 * pointed to by s1 is greater than, equal to or less than the string pointed 
 * to by s2, respectively.
 */
int strcmp(const char* s1, const char* s2);

/**
 * @brief Compare the first n characters of two strings.
 *
 * @param s1 First string
 * @param s2 Second string
 * @param n Characters to compare
 *
 * @return same as strcmp()
 */
int strncmp(const char* s1, const char* s2, size_t n);

/**
 * @brief Get the length of a string.
 *
 * @param s The string
 *
 * @return Length of the string
 */
size_t strlen(const char *s);
