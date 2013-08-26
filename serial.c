#include <avr/io.h>
#include <avr/interrupt.h>
#include "serial.h"

void (*rx_handler)(char);

void init_serial(void (*rxh)(char)) {

	// 1200 baud w/ 1MHz internal osc
	UBRRH = 0;
	UBRRL = 52;

	// 8N1
	UCSRC = (1<<URSEL)|(3<<UCSZ0);

	// Enable RX and RX Interrupt
	UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);

    rx_handler = rxh;
}

void ser_int_off() {
	UCSRB &= ~(1<<RXCIE);
}
void ser_int_on() {
	UCSRB |= (1<<RXCIE);
}

void ser_send_byte(unsigned char c) {
    if (UCSRA & (1<<UDRE))
        UDR = c;
}

ISR(SIG_UART_RECV) {
	(*rx_handler)(UDR);
}
