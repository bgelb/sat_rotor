#include <string.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "serial.h"
#include "timing.h"

int main(void)
{

	init_serial(NULL);
    init_timing();
	sei();

    PORTB = 0x00;
    PORTD = 0x00;
    DDRB = 0x38;
    DDRD = 0x40;

    ser_send_byte('h');
    tick_delay(200);    
    ser_send_byte('e');
    tick_delay(200);    
    ser_send_byte('l');
    tick_delay(200);    
    ser_send_byte('l');
    tick_delay(200);    
    ser_send_byte('o');
    tick_delay(200);

    PORTD |= 0x40;
    tick_delay(200);   
    PORTB |= 0x08;
    tick_delay(200);    
    PORTB |= 0x20;
    tick_delay(200);    
    PORTB |= 0x10;
    tick_delay(200);    
 
    PORTD &= ~(0x40);
    tick_delay(200);    
    PORTB &= ~(0x08);
    tick_delay(200);    
    PORTB &= ~(0x20);
    tick_delay(200);    
    PORTB &= ~(0x10);
    tick_delay(200);    
    
	return 0;
}
