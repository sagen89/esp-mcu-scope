#include <stdint.h>


int32_t adc_continuous_init(int32_t channel_num, int32_t sample_frequency, int32_t count_of_samples);

int32_t adc_continuous_deinitialize();

int32_t adc_continuous_start_working();

int32_t adc_continuous_stop_working();

int32_t adc_continuous_read_raw_data(int32_t buff[]);


