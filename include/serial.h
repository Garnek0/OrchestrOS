// SPDX-License-Identifier: BSD-2-Clause
/*
 * File: include/serial.h
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Description:
 * Serial driver interface.
 */

#pragma once

#include <types.h>

int serial_init(void);
char serial_char(char chr);
