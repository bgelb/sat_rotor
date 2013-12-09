// timing.c

void init_timing();
void tick_delay_ms(unsigned int ticks);
void register_10hz_tick_callback(void (*cb_func)(void *), void *cb_state);
void unregister_10hz_tick_callback();
