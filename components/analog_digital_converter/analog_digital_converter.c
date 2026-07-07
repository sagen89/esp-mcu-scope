#include "analog_digital_converter.h"
#include "sdkconfig.h"
#include "esp_log.h"
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "esp_adc/adc_continuous.h"
#include "soc/soc_caps.h"

#define ADC_UNIT (ADC_UNIT_1)
#define ADC_CONV_MODE (ADC_CONV_SINGLE_UNIT_1)
#define ADC_ATTEN (CONFIG_ADC_ATTEN)
#define ADC_BIT_WIDTH (SOC_ADC_DIGI_MAX_BITWIDTH)

static const char *TAG = "ADC";

static adc_channel_t channel[3] = {ADC_CHANNEL_4, ADC_CHANNEL_5, ADC_CHANNEL_6};
static adc_continuous_handle_t handle = NULL;

static int32_t read_len = -1;

int32_t adc_continuous_init(int32_t channel_num, int32_t sample_frequency, int32_t count_of_samples_by_ch)
{

    read_len = count_of_samples_by_ch * SOC_ADC_DIGI_DATA_BYTES_PER_CONV / SOC_ADC_DIGI_RESULT_BYTES * channel_num;
    
    adc_continuous_handle_cfg_t adc_config = {
        .max_store_buf_size = read_len,
        .conv_frame_size = read_len,
        .flags = {1},
    };

    esp_err_t is_ADC_driver_config = adc_continuous_new_handle(&adc_config, &handle);

    if (is_ADC_driver_config != ESP_OK)
    {
        ESP_ERROR_CHECK(is_ADC_driver_config);
        return -1;
    }

    adc_continuous_config_t dig_cfg = {
        .sample_freq_hz = sample_frequency,
        .conv_mode = ADC_CONV_MODE,
    };

    adc_digi_pattern_config_t adc_pattern[SOC_ADC_PATT_LEN_MAX] = {0};
    dig_cfg.pattern_num = channel_num;
    for (int i = 0; i < channel_num; i++)
    {
        adc_pattern[i].atten = ADC_ATTEN;
        adc_pattern[i].channel = channel[i] & 0x7;
        adc_pattern[i].unit = ADC_UNIT;
        adc_pattern[i].bit_width = ADC_BIT_WIDTH;

        ESP_LOGI(TAG, "adc_pattern[%d].atten is :%" PRIx8, i, adc_pattern[i].atten);
        ESP_LOGI(TAG, "adc_pattern[%d].channel is :%" PRIx8, i, adc_pattern[i].channel);
        ESP_LOGI(TAG, "adc_pattern[%d].unit is :%" PRIx8, i, adc_pattern[i].unit);
    }
    dig_cfg.adc_pattern = adc_pattern;

    esp_err_t is_ADC_IOs_config = adc_continuous_config(handle, &dig_cfg);
    if (is_ADC_IOs_config != ESP_OK)
    {
        ESP_ERROR_CHECK(is_ADC_IOs_config);
        return -1;
    }

    return (count_of_samples_by_ch * channel_num);
}

int32_t adc_continuous_start_working()
{
    esp_err_t is_start = adc_continuous_start(handle);

    if (is_start != ESP_OK)
    {
        ESP_ERROR_CHECK(is_start);
        return -1;
    }
    return 1;
}

int32_t adc_continuous_deinitialize() {
    esp_err_t is_deinit = adc_continuous_deinit(handle);

    if (is_deinit != ESP_OK)
    {
        ESP_ERROR_CHECK(is_deinit);
        return -1;
    }
    return 1;
}

int32_t adc_continuous_stop_working() {
    esp_err_t is_stop = adc_continuous_stop(handle);

    if (is_stop != ESP_OK)
    {
        ESP_ERROR_CHECK(is_stop);
        return -1;
    }
    return 1;
}

int32_t adc_continuous_read_raw_data(int32_t buff[])
{
    if (read_len <= 0) {
        return -1;
    }
    
    uint32_t ret_num = 0;
    uint8_t result[read_len];
    memset(result, 0xFF, read_len);

    esp_err_t ret = adc_continuous_read(handle, result, read_len, &ret_num, 0);

    if (ret == ESP_OK)
    {
        ESP_LOGI("TASK", "ret is %x, ret_num is %" PRIu32 " bytes", ret, ret_num);

        adc_continuous_data_t parsed_data[ret_num / SOC_ADC_DIGI_RESULT_BYTES];
        uint32_t num_parsed_samples = 0;

        esp_err_t parse_ret = adc_continuous_parse_data(handle, result, ret_num, parsed_data, &num_parsed_samples);
        if (parse_ret == ESP_OK)
        {
            for (int i = 0; i < num_parsed_samples; i++)
            {
                if (parsed_data[i].valid)
                {   
                    buff[i] = parsed_data[i].raw_data;
                }
                else
                {
                    return -1;
                }
            }
        }
        else
        {
            ESP_LOGE(TAG, "Data parsing failed: %s", esp_err_to_name(parse_ret));
            return -1;
        }
    }

    return 1;
}
