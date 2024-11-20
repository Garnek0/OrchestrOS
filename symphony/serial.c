/*
 * File: serial.c
 * 
 * Authos(s): Popa Vlad (Garnek0)
 *
 * Copyright: BSD-2-Clause
 *
 * Description:
 * Simple serial driver for the x86 PC rs232 serial interface. The only purpose
 * of this driver is to allow for logging via the serial port.
 */

#include <symphony/serial.h>
#include <symphony/arch/arch.h>

#define COM_PORT 0x3F8

#define COM_DATA COM_PORT
#define COM_INT COM_PORT+1
#define COM_DIVISOR_LSB COM_PORT
#define COM_DIVISOR_MSB COM_PORT+1
#define COM_FIFO_CTRL COM_PORT+2
#define COM_LINE_CONTROL COM_PORT+3
#define COM_MODEM_CTRL COM_PORT+4
#define COM_LINE_STATUS COM_PORT+5
#define COM_SCRATCH COM_PORT+7

int serial_init(void) {
#ifdef __x86_64__
	arch_outb(COM_INT, 0x00);
	arch_outb(COM_LINE_CONTROL, 0x80);
	arch_outb(COM_DIVISOR_LSB, 0x0C);
	arch_outb(COM_DIVISOR_MSB, 0x00);
	arch_outb(COM_LINE_CONTROL, 0x03);
	arch_outb(COM_FIFO_CTRL, 0xC7);
	arch_outb(COM_MODEM_CTRL, 0x0F);
#endif

	return 0;
}

char serial_char(char chr) {
#ifdef __x86_64__
	while ((arch_inb(COM_LINE_STATUS) & 0x20) == 0)
		continue;
	
	arch_outb(COM_DATA, chr);

	if (chr == '\n')
		serial_char('\r');
#endif

	return chr;
}
