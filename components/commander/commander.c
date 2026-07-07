#include "commander.h"
#include "analog_digital_converter.h"
#include "pwm_led.h"
#include "sdkconfig.h"
#include <stdio.h>
#include "driver/gpio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include "esp_err.h"
#include "esp_log.h"

#define GPIO_OF_ON_BOARD_LED (CONFIG_GPIO_OF_ON_BOARD_LED)
#define LEDC_TIMER_NUM (CONFIG_LEDC_TIMER_NUM)
#define LEDC_CHANNEL_NUM (CONFIG_LEDC_CHANNEL_NUM)
#define LEDC_CLOCK (LEDC_APB_CLK)
#define LEDC_CLOCK_HZ (LEDC_APB_CLK_HZ)
// #define counting_sem (xSemaphoreCreateCounting(LEDC_CHANNEL_NUM, 0))

static const char *TAG = "Commander: ";

static uint8_t led_state = 1;


static void change_level_onBoard_Led(void)
{
    /* Set the GPIO level according to the state (LOW or HIGH)*/
    gpio_set_level(GPIO_OF_ON_BOARD_LED, led_state);
    led_state = !led_state;
}

void commander_configure_on_board_led(void)
{
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1ULL << GPIO_OF_ON_BOARD_LED),
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .pull_up_en = GPIO_PULLUP_DISABLE,
    };
    gpio_config(&io_conf);
}

void commander_blink(int32_t count)
{
    if (count < 1)
    {
        change_level_onBoard_Led();
    }
    else
    {
        led_state = 0;
        change_level_onBoard_Led();
        for (uint8_t i = 0; i < count; i++)
        {
            change_level_onBoard_Led();
            vTaskDelay(pdMS_TO_TICKS(1500));
            change_level_onBoard_Led();
            vTaskDelay(pdMS_TO_TICKS(1500));
        }
    }
}



int32_t commander_configure_of_ADC_continuous(int32_t channel_num, int32_t sample_frequency, int32_t count_of_samples)
{
    return adc_continuous_init(channel_num, sample_frequency, count_of_samples);
}

int32_t commander_deinitialize_ADC_continuous() {
    return adc_continuous_deinitialize();
}

int32_t commander_start_ADC_continuous(){
    return adc_continuous_start_working();
}

int32_t commander_read_raw_data_ADC_continuous(int32_t buff[]) {
    return adc_continuous_read_raw_data(buff);
}

int32_t commander_stop_ADC_continuous(){
    return adc_continuous_stop_working();
}

int32_t commander_configure_of_PWM(int32_t timers[], int32_t channels[])
{
    return pwm_led_configure(timers, channels);
}

int32_t commander_reconfigure_of_PWM_timer(int32_t timer, int32_t freq)
{
    return pwm_led_reconfigure_timer(timer, freq);
}

int32_t commander_reconfigure_of_PWM_channel(int32_t timer, int32_t ch, int32_t gpio_num)
{
    return pwm_led_reconfigure_channel(timer, ch, gpio_num);
}


int32_t commander_set_duty_of_PWM_channel(int32_t ch, int32_t val)
{
    return pwm_led_set_duty_of_channel(ch, val);
}

