/**
 * @file serial.h
 * @author Popa Vlad (Garnek0)
 * @copyright BSD-2-Clause
 *
 * @brief
 * Serial driver interface.
 */

#pragma once

#include <symphony/types.h>

/**
 * @brief Initialize serial COM1 for logging.
 *
 * @return 0 on completion
 */
int serial_init(void);

/**
 * @brief Print a character on the serial port.
 *
 * @param chr The character to be printed 
 *
 * @return The printed character 
 */
char serial_char(char chr);
