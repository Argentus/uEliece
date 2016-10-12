/*	Atmega1284P - uEliece tester
 *-------------------------------------------
 *	File:	"main.c"
 * by author:
 *		Radovan Bezak, 2016
 *		radobezak@gmail.com
 *
 *
 *
 *
 *
 */

#define F_CPU 8000000UL		// CPU Frequency = 8MHz


// Basic AVR includes
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdlib.h>

#include "uEl_keys.h"
#include "../../../libuEliece/AVR8/uEliece.h"

// Defines for USART communication
#define USART_BAUDRATE 9600
#define BAUD_PRESCALE (((F_CPU / (USART_BAUDRATE * 16UL))) - 1)

void uart_init() {
	
	UCSR0B |= (1 << RXEN0) | (1 << TXEN0) | ( 1 << RXCIE0 );
	UCSR0C |= 1 << USBS0;
	UBRR0L = BAUD_PRESCALE;
	UBRR0H = (BAUD_PRESCALE >> 8);	

}

void uart_writeByte(uint8_t byte) {
	
	while ((UCSR0A & (1 << UDRE0)) == 0) {}; // Wait for finished transmission
      	UDR0 = byte; 

}

void uart_writeStr(char *str) {
	uint8_t i = 0;
	while(str[i]!='\0') {
		uart_writeByte(str[i]);
		++i;
	}
}

void uart_writeInt(uint16_t num, uint8_t base) {
	char buff[16];
	itoa(num, buff, base);
	uart_writeStr(buff);
}

uint8_t uart_readByte() {
	
	uint8_t received;
	while ((UCSR0A & (1 << RXC0)) == 0) {}	// Wait for reception 
	received = UDR0;
	return received;

}

ISR(USART0_RX_vect)	// On UART0 read byte - interrupt
{
}

int main() {

	sei();
	uart_init();

	uint32_t i;

	uart_writeStr("\nMeasuring encoding: \n\n");

	for (i=0; i<1; i++) {

		uEl_msglen_t ctext_len = (2 * UEL_M_PADDED )/8;
		uint8_t* msg = malloc(ctext_len);
		uart_writeInt(msg, 16);
		uEliece_encode(msg, public_key);
		
	}

	uart_writeStr("Done.\n");

	return 0;

}
