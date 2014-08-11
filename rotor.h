#define AZ_CW_ON (PORTB |= 0x08)
#define AZ_CW_OFF (PORTB &= ~(0x08))
#define AZ_CCW_ON (PORTB |= 0x04)
#define AZ_CCW_OFF (PORTB &= ~(0x04))

#define EL_UP_ON (PORTB |= 0x02)
#define EL_UP_OFF (PORTB &= ~(0x02))
#define EL_DOWN_ON (PORTB |= 0x01)
#define EL_DOWN_OFF (PORTB &= ~(0x01))

#define MIN_AZ 0
#define MAX_AZ 54500 // 54.5s = 360 deg
#define AZ_TICKS_PER_DEG 151

#define AZ_LOW_LIMIT 0
#define AZ_HI_LIMIT 360

#define MIN_EL 0
#define MAX_EL 22000 // 22s = 90 deg
#define EL_TICKS_PER_DEG 244

#define EL_LOW_LIMIT 10
#define EL_HI_LIMIT 90

#define AZ_ERR_MIN 100 // 250ms = 1.65 deg
#define AZ_ERR_MAX 2000

#define EL_ERR_MIN 100 // 250ms = 1 deg
#define EL_ERR_MAX 2000
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
int get_az_deg (rotor_state_t *s);
int get_el_deg (rotor_state_t *s);
void enable_tracking(rotor_state_t *s);
void disable_tracking(rotor_state_t *s);
void pos_controller (rotor_state_t *s);
