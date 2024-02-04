#ifndef __APP_MQTT_H_
#define __APP_MQTT_H_
#include "esp_event.h"
  
ESP_EVENT_DECLARE_BASE(MQTT_DEV_EVENT);

typedef enum 
{
    DEV_MQTT_EVENT_CONNECTED,
    DEV_MQTT_EVENT_DISCONNECTED,
    DEV_MQTT_EVENT_SUBSCRIBED,
    DEV_MQTT_EVENT_UNSUBSCRIBED,
    DEV_MQTT_EVENT_PUBLISHED,
    DEV_MQTT_EVENT_DATA
} dev_mqtt_event_id_t;

void mqtt_app_start(void);
void mqtt_publish(const char *topic, const char *data_payload);
void mqtt_subcribe(const char *topic);
void mqtt_unsubcribe(const char *topic);
#endif
