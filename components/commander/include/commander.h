#include <stdint.h>

typedef enum {
    BLINK = 101,
    ADC_CONFIGURE_CONTINUOUS = 200,
    ADC_START_CONTINUOUS = 210,
    ADC_READ_RAW_DATA_CONTINUOUS = 220,
    ADC_STOP_CONTINUOUS = 230,
    ADC_DEINITIALIZE_CONTINUOUS = 240,
    LEDC_CONFIGURE = 300,
    LEDC_RECONFIGURE_TIMER = 310,
    LEDC_RECONFIGURE_CHANNEL = 320,
    LEDC_SET_DUTY = 330,


} Commands;

void commander_configure_on_board_led(void);

void commander_blink(int32_t count);


int32_t commander_configure_of_ADC_continuous(int32_t channel_num, int32_t sample_frequency, int32_t count_of_samples);

int32_t commander_deinitialize_ADC_continuous();

int32_t commander_start_ADC_continuous();

int32_t commander_stop_ADC_continuous();

int32_t commander_read_raw_data_ADC_continuous(int32_t buff[]);

int32_t commander_configure_of_PWM(int32_t timers[], int32_t channels[]);

int32_t commander_reconfigure_of_PWM_timer(int32_t timer, int32_t freq);

int32_t commander_reconfigure_of_PWM_channel(int32_t timer, int32_t ch, int32_t gpio_num);

int32_t commander_set_duty_of_PWM_channel(int32_t ch, int32_t val);