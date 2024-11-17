// SPDX-License-Identifier: BSD-2-Clause
/*
 * File: include/string.h
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Description:
 * libc-like string and memory functions.
 */

#pragma once

#include <types.h>

void *memcpy(void *dest, const void *src, size_t n);
void *memset(void *s, int c, size_t n);
void *memmove(void *dest, const void *src, size_t n);
int memcmp(const void *s1, const void *s2, size_t n);
int strcmp(const char* s1, const char* s2);
int strncmp(const char* s1, const char* s2, size_t n);
size_t strlen(const char *s);
