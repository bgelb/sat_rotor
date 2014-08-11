#include <avr/io.h>
#include <avr/interrupt.h>
#include <stddef.h>
#include "serial.h"

volatile char *rx_buffer;
volatile unsigned int rx_buffer_len;
volatile unsigned int rx_buffer_head, rx_buffer_tail;

void init_serial() {

	// 115200 baud w/ 7.372812MHz external osc
	UBRRH = 0;
	UBRRL = 3;

	// 8N1
	UCSRC = (3<<UCSZ0);

	// Enable RX and RX Interrupt
	UCSRB = (1<<RXCIE)|(1<<RXEN)|(1<<TXEN);

}

void ser_int_off() {
	UCSRB &= ~(1<<RXCIE);
}
void ser_int_on() {
	UCSRB |= (1<<RXCIE);
}

void ser_send_byte(unsigned char c) {
    while(!(UCSRA & (1<<UDRE)));
    UDR = c;
}

void ser_send_string(const char *s) {
    unsigned int i;
    for(i=0;s[i];i++)
        ser_send_byte(s[i]);
}

void init_ser_rx(char *buf, unsigned int len) {
    rx_buffer=buf;
    rx_buffer_len=len;
    rx_buffer_head=0;
    rx_buffer_tail=0; 
}

int ser_getline(char *buf, unsigned int len) {
    unsigned int i;
    i=0;
    while(i<len-1) {

        if(rx_buffer_head != rx_buffer_tail) {
            buf[i] = rx_buffer[rx_buffer_head];
            rx_buffer_head = (rx_buffer_head + 1) % rx_buffer_len;
            if(buf[i] == '\r') break;
            i++;
        }
    }
    buf[i] = '\0';
    return i;
}

ISR(USART_RX_vect) {
    char x;
    x = UDR;
    if(rx_buffer) {
        rx_buffer[rx_buffer_tail] = x;
        rx_buffer_tail = (rx_buffer_tail + 1) % rx_buffer_len;
    }
}
