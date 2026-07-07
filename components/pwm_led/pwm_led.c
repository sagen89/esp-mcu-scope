#include "pwm_led.h"
#include "sdkconfig.h"
#include <stdio.h>
#include <math.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "esp_log.h"

#define GPIO_OF_ON_BOARD_LED (CONFIG_GPIO_OF_ON_BOARD_LED)
#define LEDC_TIMER_NUM (CONFIG_LEDC_TIMER_NUM)
#define LEDC_CHANNEL_NUM (CONFIG_LEDC_CHANNEL_NUM)
#define TIMER_FREQ_HZ (CONFIG_TIMER_FREQ_HZ)
#define LEDC_CLOCK (LEDC_APB_CLK)
#define LEDC_CLOCK_HZ (LEDC_APB_CLK_HZ)

static const char *TAG = "PWM: ";


static ledc_timer_config_t ledc_h_timerx[LEDC_TIMER_NUM] = {
    {
        .duty_resolution = LEDC_TIMER_1_BIT,
        .freq_hz = TIMER_FREQ_HZ,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_0,
        .clk_cfg = LEDC_CLOCK,
    },
    {
        .duty_resolution = LEDC_TIMER_1_BIT,
        .freq_hz = TIMER_FREQ_HZ,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .timer_num = LEDC_TIMER_1,
        .clk_cfg = LEDC_CLOCK,
    }
};

static ledc_channel_config_t ledc_h_chn[LEDC_CHANNEL_NUM] = {
    {

        .channel = LEDC_CHANNEL_0,
        .duty = 0,
        .gpio_num = 18,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_0,
        .flags.output_invert = 0
    },
    {

        .channel = LEDC_CHANNEL_1,
        .duty = 0,
        .gpio_num = 19,
        .speed_mode = LEDC_HIGH_SPEED_MODE,
        .hpoint = 0,
        .timer_sel = LEDC_TIMER_1,
        .flags.output_invert = 0
    }
};

static IRAM_ATTR bool cb_ledc_fade_end_event(const ledc_cb_param_t *param, void *user_arg)
{
    BaseType_t taskAwoken = pdFALSE;

    if (param->event == LEDC_FADE_END_EVT) {
        SemaphoreHandle_t counting_sem = (SemaphoreHandle_t) user_arg;
        xSemaphoreGiveFromISR(counting_sem, &taskAwoken);
    }

    return (taskAwoken == pdTRUE);
}



int32_t pwm_led_configure(int32_t timers[], int32_t channels[])
{
    for (uint8_t num = 0; num < LEDC_TIMER_NUM; num++)
    {
        ledc_h_timerx[num].duty_resolution = ledc_find_suitable_duty_resolution(LEDC_CLOCK_HZ, ledc_h_timerx[num].freq_hz);
        esp_err_t is_timer_config = ledc_timer_config(&ledc_h_timerx[num]);
        
        if (is_timer_config != ESP_OK)
        {
            ESP_ERROR_CHECK(is_timer_config);
            return -1;
        }

        timers[num * 3] = (int32_t)num;
        timers[num * 3 + 1] = (int32_t)ledc_h_timerx[num].freq_hz;
        timers[num * 3 + 2] = (int32_t)ledc_h_timerx[num].duty_resolution;
        ESP_LOGI(TAG, "Temer %d: ferq-%d,resol-%d", num, ledc_h_timerx[num].freq_hz, ledc_h_timerx[num].duty_resolution);
    }

    for (uint8_t num = 0; num < LEDC_CHANNEL_NUM; num++)
    {
        esp_err_t is_ch_config = ledc_channel_config(&ledc_h_chn[num]);

        if (is_ch_config != ESP_OK)
        {
            ESP_ERROR_CHECK(is_ch_config);
            return -1;
        }
        channels[num * 3] = (int32_t)num;
        channels[num * 3 + 1] = (int32_t)ledc_h_chn[num].gpio_num;
        channels[num * 3 + 2] = (int32_t)ledc_h_chn[num].timer_sel;
        ESP_LOGI(TAG, "Channel %d: gpio-%d, timer-%d", num, ledc_h_chn[num].gpio_num, ledc_h_chn[num].timer_sel);
    }


    ledc_fade_func_install(0);
    ledc_cbs_t callbacks = {
        .fade_cb = cb_ledc_fade_end_event
    };
    SemaphoreHandle_t counting_sem = xSemaphoreCreateCounting(LEDC_CHANNEL_NUM, 0);

    for (uint8_t num = 0; num < LEDC_CHANNEL_NUM; num++) {
        ledc_cb_register(ledc_h_chn[num].speed_mode, ledc_h_chn[num].channel, &callbacks, (void *) counting_sem);
    }

    return 1;
}

int32_t pwm_led_reconfigure_timer(int32_t timer, int32_t freq)
{
    ledc_h_timerx[timer].freq_hz = freq;
    ledc_h_timerx[timer].duty_resolution = ledc_find_suitable_duty_resolution(LEDC_CLOCK_HZ, freq);
    esp_err_t is_timer_config = ledc_timer_config(&ledc_h_timerx[timer]);

    if (is_timer_config != ESP_OK)
    {
        ESP_ERROR_CHECK(is_timer_config);
        return -1;
    }

    return pow(2, ledc_h_timerx[timer].duty_resolution);
}

int32_t pwm_led_reconfigure_channel(int32_t timer, int32_t ch, int32_t gpio_num)
{
    ledc_h_chn[ch].timer_sel = ledc_h_timerx[timer].timer_num;
    ledc_h_chn[ch].gpio_num = gpio_num;
    esp_err_t is_ch_config = ledc_channel_config(&ledc_h_chn[ch]);

    if (is_ch_config != ESP_OK)
    {
        ESP_ERROR_CHECK(is_ch_config);
        return -1;
    }

    return 1;
}


int32_t pwm_led_set_duty_of_channel(int32_t ch, int32_t val)
{
   

    esp_err_t is_set_duty = ledc_set_duty_and_update(ledc_h_chn[ch].speed_mode,ledc_h_chn[ch].channel, val, 0);

    if (is_set_duty != ESP_OK)
    {
        ESP_ERROR_CHECK(is_set_duty);
        return -1;
    }

    return 1;
}

