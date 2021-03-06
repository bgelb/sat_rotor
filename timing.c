#include <avr/interrupt.h>
#include <avr/io.h>
#include <stddef.h>

#include "timing.h"

volatile unsigned int delay;
volatile unsigned char freq_div;
void (*cb_func_10hz)(void *);
void *cb_state_10hz;

void init_timing() {
	TCCR1A = 0x00;
	TCCR1B = 0x01; // Timer 0 at 7.372812MHz
	TIMSK = 1<<TOIE1; // Turn on interrupt for Timer 0
    TCNT1 = 65535 - 7373;
    cb_func_10hz = NULL;
    cb_state_10hz = NULL;
    freq_div = 0;
}

void tick_delay_ms(unsigned int ticks) {
	delay = ticks;

	while(delay>0) {
	}

	return;
}

void register_10hz_tick_callback(void (*cb_func)(void *), void *cb_state) {
    cb_func_10hz = cb_func;
    cb_state_10hz = cb_state;
}
void unregister_10hz_tick_callback() {
    cb_func_10hz = NULL;
    cb_state_10hz = NULL;
}

SIGNAL(TIMER1_OVF_vect) {
	TCNT1 = 65535 - 7373; // 1ms interval
	
    if(delay>0) delay--;
    
    if(freq_div == 99) freq_div = 0;
    else freq_div++;

    if(freq_div==0) {
        if(cb_func_10hz) cb_func_10hz(cb_state_10hz);
    }
}
