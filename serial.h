void init_serial();
void ser_int_off();
void ser_int_on();
void ser_send_byte(unsigned char c);
void ser_send_string(const char *s);
void init_ser_rx(char *buf, unsigned int len);
int ser_getline(char *buf, unsigned int len);
