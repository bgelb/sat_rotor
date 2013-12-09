#include <avr/io.h>

#include "rotor.h"
#include "timing.h"

#define MIN2(a,b) (a > b ? b : a)

void reinit_rotor_pos (rotor_state_t *s) {
    unsigned int i;

    s->track_target=0;

    AZ_CW_OFF;
    AZ_CCW_OFF;
    EL_UP_OFF;
    EL_DOWN_OFF;

    tick_delay(2);

    AZ_CCW_ON;
    EL_UP_ON;

    for(i=0;i<(MAX_AZ/1000+5);i++) {
        tick_delay(1); //10;
    }

    AZ_CCW_OFF;
    EL_UP_OFF;
    
    s->pos.az_ticks = 0;
    s->pos.el_ticks = MAX_EL;
    s->target_pos.az_ticks = 0;
    s->target_pos.el_ticks = MAX_EL;
    s->az_drive_state = AZ_IDLE;
    s->el_drive_state = EL_IDLE;
}

void set_target_az_deg (rotor_state_t *s, unsigned int az) {
    if(az >= 0 && az <= 360)
        s->target_pos.az_ticks = (int)(az*(MAX_AZ/360.0)); // lookup table?
}

void set_target_el_deg (rotor_state_t *s, unsigned int el) {
    if(el >= 0 && el <= 90)
        s->target_pos.el_ticks = (int)(el*(MAX_EL/90.0)); // lookup table?
}

void enable_tracking (rotor_state_t *s) {
    s->track_target = 1;
}

void disable_tracking (rotor_state_t *s) {
    s->track_target = 0;
}

void pos_controller (rotor_state_t *s) {
    //
    // Update position
    //

    // AZ
    if(s->az_drive_state == AZ_CW && s->pos.az_ticks < MAX_AZ)
        s->pos.az_ticks = MIN2(s->pos.az_ticks + 100, MAX_AZ); // 1 tick = 100ms
    else if(s->az_drive_state == AZ_CCW && s->pos.az_ticks > 0)
        s->pos.az_ticks = s->pos.az_ticks - MIN2(s->pos.az_ticks, 100);

    // EL
    if(s->el_drive_state == EL_UP && s->pos.az_ticks < MAX_EL)
        s->pos.el_ticks = MIN2(s->pos.el_ticks + 100, MAX_EL); // 1 tick = 100ms
    else if(s->el_drive_state == EL_DOWN && s->pos.el_ticks > 0)
        s->pos.el_ticks = s->pos.el_ticks - MIN2(s->pos.el_ticks, 100);

    //
    // Check error vs. target, set drive state
    //
    // always transition through IDLE state, so motors can't be
    // driven in both directions simultaneously
    //

    if(s->track_target) {
        // AZ
        if(s->target_pos.az_ticks > s->pos.az_ticks + AZ_ERR_MAX) { // target CW
            if(s->az_drive_state == AZ_IDLE) {
                s->az_drive_state = AZ_CW;
                AZ_CW_ON;
            }
            else if(s->az_drive_state == AZ_CCW) {
                s->az_drive_state = AZ_IDLE;
                AZ_CCW_OFF;
            }
        }
        else if(s->target_pos.az_ticks + AZ_ERR_MAX < s->pos.az_ticks) { // target CCW
            if(s->az_drive_state == AZ_IDLE) {
                s->az_drive_state = AZ_CCW;
                AZ_CCW_ON;
            }
            else if(s->az_drive_state == AZ_CW) {
                s->az_drive_state = AZ_IDLE;
                AZ_CW_OFF;
            }
        }
        else { // within target error
            s->az_drive_state = AZ_IDLE;
            AZ_CW_OFF; AZ_CCW_OFF;
        } 

        // EL
        if(s->target_pos.el_ticks > s->pos.el_ticks + EL_ERR_MAX) { // target UP
            if(s->el_drive_state == EL_IDLE) {
                s->el_drive_state = EL_UP;
                EL_UP_ON;
            }
            else if(s->el_drive_state == EL_DOWN) {
                s->el_drive_state = EL_IDLE;
                EL_DOWN_OFF;
            }
        }
        else if(s->target_pos.el_ticks + EL_ERR_MAX < s->pos.el_ticks) { // target DOWN
            if(s->el_drive_state == EL_IDLE) {
                s->el_drive_state = EL_DOWN;
                AZ_CCW_ON;
            }
            else if(s->el_drive_state == EL_UP) {
                s->el_drive_state = EL_IDLE;
                EL_UP_OFF;
            }
        }
        else { // within target error
            s->el_drive_state = EL_IDLE;
            EL_UP_OFF; EL_DOWN_OFF;
        }
    } 
}
