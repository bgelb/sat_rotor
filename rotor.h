#define AZ_CW_ON (PORTD |= 0x40)
#define AZ_CW_OFF (PORTD &= ~(0x40))
#define AZ_CCW_ON (PORTB |= 0x08)
#define AZ_CCW_OFF (PORTB &= ~(0x08))

#define EL_UP_ON (PORTB |= 0x20)
#define EL_UP_OFF (PORTB &= ~(0x20))
#define EL_DOWN_ON (PORTB |= 0x10)
#define EL_DOWN_OFF (PORTB &= ~(0x10))

#define MIN_AZ 0
#define MAX_AZ 54500 // 54.5s = 360 deg

#define MIN_EL 0
#define MAX_EL 22000 // 22s = 90 deg

#define AZ_ERR_MAX 250 // 250ms = 1.65 deg
#define EL_ERR_MAX 250 // 250ms = 1 deg

typedef enum {
    AZ_IDLE,
    AZ_CW,
    AZ_CCW
} az_drive_state_t;

typedef enum {
    EL_IDLE,
    EL_UP,
    EL_DOWN
} el_drive_state_t;

typedef struct {
    long az_ticks;
    long el_ticks;
} rotor_pos_t;

typedef struct {
    rotor_pos_t pos;
    rotor_pos_t target_pos;
    unsigned char track_target;
    //
    az_drive_state_t az_drive_state;
    el_drive_state_t el_drive_state;
} rotor_state_t;

void reinit_rotor_pos (rotor_state_t *s);
void set_target_az_deg (rotor_state_t *s, unsigned int az);
void set_target_el_deg (rotor_state_t *s, unsigned int el);
void enable_tracking(rotor_state_t *s);
void disable_tracking(rotor_state_t *s);
void pos_controller (rotor_state_t *s);
