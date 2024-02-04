
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "protocol_examples_common.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"

#include "esp_log.h"
#include "mqtt_client.h"

#include "ubidotsEsp32Mqtt.h"
#include "esp_output.h"

const char DEVICE_LABEL[]               = "esp32";  
#define VARIABLE_LABEL_TEMPERATURE      "temperature"
#define VARIABLE_LABEL_HUMIDITY         "humidity"
#define TIME_DELAY                      (2000)

static const char *TAG = "MQTT_MAIN";

static float temp = 1.0;
static float humi = 1.0;

static uint8_t flag_subcribed = 0;

static void mqtt_dev_event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == MQTT_DEV_EVENT)
    {
        if (event_id == DEV_MQTT_EVENT_CONNECTED)
        {
            ubidots_subcribe("esp32", "switch_on");
            ESP_LOGW(TAG, "DEV_MQTT_EVENT_CONNECTED");
        }
        else if (event_id == DEV_MQTT_EVENT_DISCONNECTED)
        {
            flag_subcribed = 0;
            ubidots_unsubcribe("esp32", "switch_on");
            ESP_LOGW(TAG, "DEV_MQTT_EVENT_DISCONNECTED");
        }
        else if (event_id == DEV_MQTT_EVENT_SUBSCRIBED)
        {
            flag_subcribed = 1;
            ESP_LOGW(TAG, "DEV_MQTT_EVENT_SUBSCRIBED");  
        }
        else if (event_id == DEV_MQTT_EVENT_UNSUBSCRIBED)
        {
            ESP_LOGW(TAG, "DEV_MQTT_EVENT_UNSUBSCRIBED");  
        }
        else if (event_id == DEV_MQTT_EVENT_PUBLISHED)
        {
            ESP_LOGW(TAG, "DEV_MQTT_EVENT_PUBLISHED");
        }
        else if (event_id == DEV_MQTT_EVENT_DATA)
        {
            ESP_LOGW(TAG, "DEV_MQTT_EVENT_DATA");
            printf("DATA=%s\r\n", (char *)event_data);

            if (strstr((char *)event_data, "0.0") != NULL)
            {
                esp_output_set_level(2, 0);
            }
            else if (strstr((char *)event_data, "1.0") != NULL)
            {
                esp_output_set_level(2, 1);
            }
        }
    }
}


void app_main(void)
{
    ESP_LOGI(TAG, "[APP] Startup..");
    ESP_LOGI(TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
    ESP_LOGI(TAG, "[APP] IDF version: %s", esp_get_idf_version());

    ESP_ERROR_CHECK(nvs_flash_init());
    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    

    esp_output_init(2);
    esp_output_set_level(2,0);

    ESP_ERROR_CHECK(example_connect());
    
    ubidots_init();

    esp_event_handler_register(MQTT_DEV_EVENT, ESP_EVENT_ANY_ID, &mqtt_dev_event_handler, NULL);
    vTaskDelay(TIME_DELAY / portTICK_PERIOD_MS);

    while (1)
    {
        if (flag_subcribed)
        {
            ubidots_publish("esp32", "temperature", temp);
            // Bat buoc can 1 khoang delay (it nhat >= 600 ms) giua 2 lan Publish 
            vTaskDelay(TIME_DELAY / portTICK_PERIOD_MS);
            ubidots_publish("esp32", "humidity", humi);
            vTaskDelay(TIME_DELAY / portTICK_PERIOD_MS);
            temp += 0.5;
            humi += 0.5;
            if (temp >= 100 || humi >= 100)
            {
                temp = 1.0;
                humi = 1.0;
            }
        }
        
    }
}
