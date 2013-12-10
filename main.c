#include <string.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include "serial.h"
#include "timing.h"
#include "rotor.h"

char rxb[128];
char lineb[64];
char obuf[64];

int main(void)
{
    rotor_state_t rs;

    init_ser_rx(rxb, 128);
	init_serial();
    init_timing();
	sei();

    PORTB = 0x00;
    PORTD = 0x00;
    DDRB = 0x38;
    DDRD = 0x40;

    reinit_rotor_pos(&rs);

    register_10hz_tick_callback((void(*)(void *))pos_controller, (void *)&rs);
    enable_tracking(&rs);

    unsigned int deg;

    while(1) {
        ser_getline(lineb, 64);
  
        if(lineb[0] == 'C' && lineb[1] == '\0') {
            sprintf(obuf, "+0%03d\r\n", get_az_deg(&rs));
            ser_send_string(obuf);
        }
        else if(lineb[0] == 'C' && lineb[1] == '2') {
            sprintf(obuf, "+0%03d+0%03d\r\n", get_az_deg(&rs), get_el_deg(&rs));
            ser_send_string(obuf);
        }
        else if(lineb[0] == 'B' && lineb[1] == '\0') {
            sprintf(obuf, "+0%03d\r\n", get_el_deg(&rs));
            ser_send_string(obuf);
        }
        else if(lineb[0] == 'M') {
            deg=(lineb[1]-48)*100;
            deg+=(lineb[2]-48)*10;
            deg+=(lineb[3]-48);
            set_target_az_deg(&rs, deg);
        }
        else if(lineb[0] == 'W') {
            deg=(lineb[1]-48)*100;
            deg+=(lineb[2]-48)*10;
            deg+=(lineb[3]-48);
            set_target_az_deg(&rs, deg);

            deg=(lineb[5]-48)*100;
            deg+=(lineb[6]-48)*10;
            deg+=(lineb[7]-48);
            set_target_el_deg(&rs, deg);
        }
        else {
            ser_send_string("?>\r");
        }
    }

	return 0;
}
