#include <string.h>
#include <avr/interrupt.h>
#include <avr/io.h>
#include <stdio.h>
#include "serial.h"
#include "timing.h"
#include "rotor.h"

char rxb[32];
char lineb[16];
char obuf[16];

int main(void)
{
    rotor_state_t rs;

    init_ser_rx(rxb, 32);
	init_serial();
    init_timing();
	sei();

    PORTB = 0x00;
    DDRB = 0x0F;

    ser_send_string("serial up\r");
    reinit_rotor_pos(&rs);

    register_10hz_tick_callback((void(*)(void *))pos_controller, (void *)&rs);
    enable_tracking(&rs);

    unsigned int deg;

    while(1) {
        ser_getline(lineb, 16);
  
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
        //else if(lineb[0] == 'D' && lineb[1] == '\0') {
        //    sprintf(obuf, "az: tgt=%ld cur=%ld\r\nel: tgt=%ld cur=%ld\r\n",
        //                            rs.target_pos.az_ticks,
        //                            rs.pos.az_ticks,
        //                            rs.target_pos.el_ticks,
        //                            rs.pos.el_ticks);
        //    ser_send_string(obuf);
        //    sprintf(obuf, "el_drive: %s\r\n",
        //        (rs.el_drive_state==EL_UP ? "EL_UP" :
        //            (rs.el_drive_state==EL_DOWN ? "EL_DOWN" : "EL_IDLE")));
        //    ser_send_string(obuf);
        //    sprintf(obuf, "az_drive: %s\r\n",
        //        (rs.az_drive_state==AZ_CW ? "AZ_CW" :
        //            (rs.az_drive_state==AZ_CCW ? "AZ_CCW" : "AZ_IDLE")));
        //    ser_send_string(obuf);
        //}
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
