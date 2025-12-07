#include "commander.h"
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define GPIO_OF_ON_BOARD_LED (CONFIG_GPIO_OF_ON_BOARD_LED)

static uint8_t led_state = 1;

void commander_configure_on_board_led(void) {
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << GPIO_OF_ON_BOARD_LED),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&io_conf);
}


static void change_level_onBoard_Led(void) {
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(GPIO_OF_ON_BOARD_LED, led_state);
    led_state = !led_state;
}

void commander_blink(int32_t count) {
    if (count < 1) {
        change_level_onBoard_Led();
    } else {
        led_state = 0;
        change_level_onBoard_Led();
        for (uint8_t i = 0; i < count; i++) {
            change_level_onBoard_Led();
            vTaskDelay(pdMS_TO_TICKS(1500));
            change_level_onBoard_Led();
            vTaskDelay(pdMS_TO_TICKS(1500));
        }
    }
}

