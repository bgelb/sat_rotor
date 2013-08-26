#include <avr/interrupt.h>
#include <avr/io.h>

#include "timing.h"

volatile unsigned char delay;

void init_timing() {
	TCCR1A = 0x00;
	TCCR1B = 0x05; // Timer 0 at 1kHz
	TIMSK = 1<<TOIE1; // Turn on interrupt for Timer 0
}

void tick_delay(unsigned short ticks) {
	delay = 1;
	TCNT1 = 65535 - ticks;

	while(delay) {
	}

	return;
}

SIGNAL(SIG_OVERFLOW1) {
	delay = 0;
	//TCNT1 = 0;
}
