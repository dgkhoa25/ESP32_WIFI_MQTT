#include "app_mqtt.h"
#include <string.h>
#include <strings.h>
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

ESP_EVENT_DEFINE_BASE(MQTT_DEV_EVENT);


static const char *TAG = "APP_MQTT";

EventGroupHandle_t xEventGroup = NULL;
#define PUBLISH_DONE_BIT	        ( 1 << 0 )
#define SUBCRIBE_DONE_BIT	        ( 1 << 1 )
#define UNSUBCRIBE_DONE_BIT	        ( 1 << 2 )

esp_mqtt_client_handle_t client = NULL;


static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    ESP_LOGD(TAG, "Event dispatched from event loop base=%s, event_id=%d", base, event_id);

    esp_mqtt_event_handle_t event = event_data;

    switch ((esp_mqtt_event_id_t)event_id) 
    {
        case MQTT_EVENT_CONNECTED:

            ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
            esp_event_post(MQTT_DEV_EVENT, DEV_MQTT_EVENT_CONNECTED, NULL, 0, portMAX_DELAY);
            break;

        case MQTT_EVENT_DISCONNECTED:

            ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
            esp_event_post(MQTT_DEV_EVENT, DEV_MQTT_EVENT_DISCONNECTED, NULL, 0, portMAX_DELAY);
            break;
        case MQTT_EVENT_SUBSCRIBED:

            ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
            xEventGroupSetBits(xEventGroup, SUBCRIBE_DONE_BIT);
            esp_event_post(MQTT_DEV_EVENT, DEV_MQTT_EVENT_SUBSCRIBED, NULL, 0, portMAX_DELAY);
            break;
        case MQTT_EVENT_UNSUBSCRIBED:

            ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
            xEventGroupSetBits(xEventGroup, UNSUBCRIBE_DONE_BIT);
            esp_event_post(MQTT_DEV_EVENT, DEV_MQTT_EVENT_UNSUBSCRIBED, NULL, 0, portMAX_DELAY);
            break;
        case MQTT_EVENT_PUBLISHED:

            ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
            xEventGroupSetBits(xEventGroup, PUBLISH_DONE_BIT);
            // esp_event_post(MQTT_DEV_EVENT, DEV_MQTT_EVENT_PUBLISHED, NULL, 0, portMAX_DELAY);
            break;
        case MQTT_EVENT_DATA:
            printf("TOPIC=%.*s\r\n", event->topic_len, event->topic);
            printf("DATA=%.*s\r\n", event->data_len, event->data);
            ESP_LOGI(TAG, "MQTT_EVENT_DATA");
            esp_event_post(MQTT_DEV_EVENT, DEV_MQTT_EVENT_DATA, event->data, sizeof((char *)event->data) , portMAX_DELAY);

            break;
        default:
            ESP_LOGI(TAG, "Other event id:%d", event->event_id);
            break;
    }
}

void mqtt_app_start(void)
{

    const esp_mqtt_client_config_t mqtt_cfg = 
    {
        .uri = "mqtt://industrial.api.ubidots.com:1883",
        .username = "BBUS-JYvWWEnXQhWOBYyvgAR3hdv83vnuJK"
    };

    client = esp_mqtt_client_init(&mqtt_cfg);
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);
    esp_mqtt_client_start(client);
}

void mqtt_publish(const char *topic, const char *data_payload)
{
    xEventGroup = xEventGroupCreate();
    esp_mqtt_client_publish(client, topic, data_payload, 0, 1, 0);

    xEventGroupWaitBits( xEventGroup, PUBLISH_DONE_BIT, pdTRUE, pdFALSE, portMAX_DELAY );

    vEventGroupDelete(xEventGroup);
}

void mqtt_subcribe(const char *topic)
{
    xEventGroup = xEventGroupCreate();
    esp_mqtt_client_subscribe(client, topic, 1);

    xEventGroupWaitBits( xEventGroup, SUBCRIBE_DONE_BIT, pdTRUE, pdFALSE, portMAX_DELAY );

    vEventGroupDelete(xEventGroup);
}

void mqtt_unsubcribe(const char *topic)
{
    xEventGroup = xEventGroupCreate();
    esp_mqtt_client_unsubscribe(client, topic);

    xEventGroupWaitBits( xEventGroup, UNSUBCRIBE_DONE_BIT, pdTRUE, pdFALSE, portMAX_DELAY );

    vEventGroupDelete(xEventGroup);
}