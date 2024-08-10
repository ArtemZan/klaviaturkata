void keypad_init();

void keypad_get_values(uint16_t values[12]);

void keypad_get_base_values(unsigned int samples_count);

extern uint64_t keypad_base_values[12];