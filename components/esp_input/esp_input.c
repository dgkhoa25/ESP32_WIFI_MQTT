#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_input.h"


static input_callback_t input_callback = NULL;
static xQueueHandle gpio_evt_queue = NULL;

static uint32_t start_tick = 0;
static uint32_t end_tick = 0;
static uint32_t delta_tick = 0;

int esp_input_get_level(int pin)
{
    return gpio_get_level(pin);
}

static void IRAM_ATTR gpio_isr_handler(void* arg)
{
    uint32_t gpio_num = (uint32_t) arg;
    
    if (esp_input_get_level(gpio_num) == 0)
    {
        start_tick = xTaskGetTickCountFromISR();
    }
    else
    {
        end_tick = xTaskGetTickCountFromISR();
        delta_tick = end_tick - start_tick;
        xQueueSendFromISR(gpio_evt_queue, &gpio_num, NULL);
    }
}

static void gpio_itr_isr(void* arg)
{
    uint32_t io_num;
    for(;;) {
        if(xQueueReceive(gpio_evt_queue, &io_num, portMAX_DELAY)) {
            input_callback((int)io_num, delta_tick);
        }
    }
}




void esp_input_init(int pin)
{
    //  zero-initialize the config structure.
    gpio_config_t io_conf = {};

    //  interrupt of falling edge
    io_conf.intr_type = GPIO_INTR_NEGEDGE;
    //  bit mask of the pins
    io_conf.pin_bit_mask = (1UL << pin);
    //  set as input mode
    io_conf.mode = GPIO_MODE_INPUT;
    //  enable pull-up mode
    io_conf.pull_up_en = 1;
    gpio_config(&io_conf);

    //  hange gpio intrrupt type for one pin
    // gpio_set_intr_type(pin, GPIO_INTR_NEGEDGE);
    gpio_set_intr_type(pin, GPIO_INTR_ANYEDGE);

    //  install gpio isr service
    gpio_install_isr_service(0);
    //  ook isr handler for specific gpio pin
    gpio_isr_handler_add(pin, gpio_isr_handler, (void*) pin);

    //  remove isr handler for gpio number.
    gpio_isr_handler_remove(pin);
    //  hook isr handler for specific gpio pin again
    gpio_isr_handler_add(pin, gpio_isr_handler, (void*) pin);

    //  create a queue to handle gpio event from isr
    gpio_evt_queue = xQueueCreate(10, sizeof(uint32_t));
    //  start gpio task
    xTaskCreate(gpio_itr_isr, "gpio_itr_isr", 2048, NULL, 10, NULL);
}

void esp_input_set_callback(void *cb)
{
    if (cb != NULL)
    {
        input_callback = cb; // Point to external main function
    }
}