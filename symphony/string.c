// SPDX-License-Identifier: BSD-2-Clause
/*
 * File: string.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Description:
 * Implementations of various memory- and string-related functions from the 
 * standard C library. Some of these (memcpy, memset, memmove and memcmp) are
 * needed by some compilers (including GCC) and must be implemented in
 * freestanding code. 
 */

#include <symphony/types.h>

void* memcpy(void* dest, const void* src, size_t n) {
	uint8_t* pdest = (uint8_t*)dest;
	const uint8_t* psrc = (const uint8_t*)src;

	for (size_t i = 0; i < n; i++)
		pdest[i] = psrc[i];

	return dest;
}

void* memset(void* s, int c, size_t n) {
	uint8_t* p = (uint8_t*)s;

	for (size_t i = 0; i < n; i++)
		p[i] = (uint8_t)c;

	return s;
}

void* memmove(void* dest, const void* src, size_t n) {
	uint8_t* pdest = (uint8_t*)dest;
	const uint8_t* psrc = (const uint8_t*)src;

	if (src > dest) {
		for (size_t i = 0; i < n; i++)
			pdest[i] = psrc[i];
	} else if (src < dest) {
		for (size_t i = n; i > 0; i--)
			pdest[i-1] = psrc[i-1];
	}

	return dest;
}

int memcmp(const void* s1, const void* s2, size_t n) {
	const uint8_t* p1 = (const uint8_t*)s1;
	const uint8_t* p2 = (const uint8_t*)s2;

	for (size_t i = 0; i < n; i++) {
		if (p1[i] != p2[i])
			return p1[i] < p2[i] ? -1 : 1;
	}

	return 0;
}

int strcmp(const char* s1, const char* s2) {
   while (*s1 != '\0' && *s2 != '\0'  && *s1 == *s2) {
	  s1++;
	  s2++;
   }

   return *s1 - *s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
   while (n != 0 && *s1 != '\0' && *s2 != '\0'	&& *s1 == *s2) {
	  s1++;
	  s2++;
	  n--;
   }

   return n == 0 ? 0 : (*s1 - *s2);
}

size_t strlen(const char* s) {
	size_t count = 0;

	while(*s!=0) {
		count++;
		s++;
	}

	return count;
}
