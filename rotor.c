#include <avr/io.h>

#include "rotor.h"
#include "timing.h"

#define MIN2(a,b) ((a) > (b) ? (b) : (a))

void reinit_rotor_pos (rotor_state_t *s) {
    s->track_target=0;

    AZ_CW_OFF;
    AZ_CCW_OFF;
    EL_UP_OFF;
    EL_DOWN_OFF;

    tick_delay_ms(200);

    AZ_CCW_ON;
    EL_UP_ON;

    tick_delay_ms(MAX_AZ+5000);

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
    if(az >= 0 && az <= 360) {
        if(az < AZ_LOW_LIMIT) az = AZ_LOW_LIMIT;
        else if(az > AZ_HI_LIMIT) az = AZ_HI_LIMIT;
        s->target_pos.az_ticks = az*AZ_TICKS_PER_DEG; // lookup table?
    }
}

void set_target_el_deg (rotor_state_t *s, unsigned int el) {
    if(el >= 0 && el <= 90) {
        if(el < EL_LOW_LIMIT) el = EL_LOW_LIMIT;
        else if(el > EL_HI_LIMIT) el = EL_HI_LIMIT;
        s->target_pos.el_ticks = el*EL_TICKS_PER_DEG; // lookup table?
    }
}

int get_az_deg (rotor_state_t *s) {
    return (int)(s->pos.az_ticks/AZ_TICKS_PER_DEG);
}

int get_el_deg (rotor_state_t *s) {
    return (int)(s->pos.el_ticks/EL_TICKS_PER_DEG);
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
    if(s->el_drive_state == EL_UP && s->pos.el_ticks < MAX_EL)
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
        if(s->target_pos.az_ticks > s->pos.az_ticks + AZ_ERR_MIN) { // target CW
            if(s->az_drive_state == AZ_IDLE) {
                if(s->target_pos.az_ticks > s->pos.az_ticks + AZ_ERR_MAX) {
                    s->az_drive_state = AZ_CW;
                    AZ_CW_ON;
                }
            }
            else if(s->az_drive_state == AZ_CCW) {
                s->az_drive_state = AZ_IDLE;
                AZ_CCW_OFF;
            }
        }
        else if(s->target_pos.az_ticks + AZ_ERR_MIN < s->pos.az_ticks) { // target CCW
            if(s->az_drive_state == AZ_IDLE) {
                if(s->target_pos.az_ticks + AZ_ERR_MAX < s->pos.az_ticks) {
                    s->az_drive_state = AZ_CCW;
                    AZ_CCW_ON;
                }
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
        if(s->target_pos.el_ticks > s->pos.el_ticks + EL_ERR_MIN) { // target UP
            if(s->el_drive_state == EL_IDLE) {
                if(s->target_pos.el_ticks > s->pos.el_ticks + EL_ERR_MAX) {
                    s->el_drive_state = EL_UP;
                    EL_UP_ON;
                }
            }
            else if(s->el_drive_state == EL_DOWN) {
                s->el_drive_state = EL_IDLE;
                EL_DOWN_OFF;
            }
        }
        else if(s->target_pos.el_ticks + EL_ERR_MIN < s->pos.el_ticks) { // target DOWN
            if(s->el_drive_state == EL_IDLE) {
                if(s->target_pos.el_ticks + EL_ERR_MAX < s->pos.el_ticks) {
                    s->el_drive_state = EL_DOWN;
                    EL_DOWN_ON;
                }
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
