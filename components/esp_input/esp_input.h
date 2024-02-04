#ifndef esp_input_h
#define esp_input_h

typedef void (*input_callback_t)(int, uint32_t);

void esp_input_init(int pin);
int  esp_input_get_level(int pin);
void esp_input_set_callback(void *cb);

#endif
