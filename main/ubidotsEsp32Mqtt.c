#include "ubidotsEsp32Mqtt.h"
#include "app_mqtt.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "esp_log.h"

static const char *TAG = "UBIDOTS";

void ubidots_init(void)
{
    mqtt_app_start();
}


void ubidots_publish(const char* deviceLabel, const char* variableLabel, float value)
{
    char topic[150];
    char payload[30];

    
    sprintf(topic, "/v1.6/devices/%s/%s", deviceLabel, variableLabel);

    sprintf(payload,  "{\"value\":%02.02f}", value);  

    mqtt_publish(topic, payload);
}

void ubidots_subcribe(const char* deviceLabel, const char* variableLabel)
{
    char topic[150];

    sprintf(topic, "/v1.6/devices/%s/%s/lv", deviceLabel, variableLabel);

    mqtt_subcribe(topic);
}

void ubidots_unsubcribe(const char* deviceLabel, const char* variableLabel)
{
    char topic[150];

    sprintf(topic, "/v1.6/devices/%s/%s/lv", deviceLabel, variableLabel);

    mqtt_unsubcribe(topic);
}

