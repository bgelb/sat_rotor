void init_serial(void (*rxh)(char));
void ser_int_off();
void ser_int_on();
void ser_send_byte(unsigned char c);
