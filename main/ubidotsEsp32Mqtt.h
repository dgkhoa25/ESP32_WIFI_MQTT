#ifndef __UBIDOTSESP32MQTT_H_
#define __UBIDOTSESP32MQTT_H_
#include "app_mqtt.h"

void ubidots_init(void);
void ubidots_publish(const char* deviceLabel, const char* variableLabel, float value);
void ubidots_subcribe(const char* deviceLabel, const char* variableLabel);
void ubidots_unsubcribe(const char* deviceLabel, const char* variableLabel);
#endif
