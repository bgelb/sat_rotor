#include <string.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include "serial.h"
#include "timing.h"
#include "rotor.h"

int main(void)
{
    rotor_state_t rs;

	init_serial(NULL);
    init_timing();
	sei();

    PORTB = 0x00;
    PORTD = 0x00;
    DDRB = 0x38;
    DDRD = 0x40;

    //ser_send_byte('h');
    //tick_delay(200);    
    //ser_send_byte('e');
    //tick_delay(200);    
    //ser_send_byte('l');
    //tick_delay(200);    
    //ser_send_byte('l');
    //tick_delay(200);    
    //ser_send_byte('o');
    //tick_delay(200);

    //AZ_CW_ON;
    //tick_delay(200);   
    //PORTB |= 0x08;
    //tick_delay(200);    
    //PORTB |= 0x20;
    //tick_delay(200);    
    //PORTB |= 0x10;
    //tick_delay(200);    
 
    //AZ_CW_OFF;
    //tick_delay(200);    
    //PORTB &= ~(0x08);
    //tick_delay(200);    
    //PORTB &= ~(0x20);
    //tick_delay(200);    
    //PORTB &= ~(0x10);
    //tick_delay(200);    
    
    reinit_rotor_pos(&rs);

    register_10hz_tick_callback(pos_controller, (void *)&rs);
    enable_tracking(&rs);

    tick_delay(10); // 1s

    set_target_el_deg(&rs, 30); // should be approx 7 sec
    set_target_az_deg(&rs, 30); // should be approx 5 sec

    while(1);

	return 0;
}
