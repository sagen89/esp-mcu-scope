#include <stdint.h>

typedef enum {
    BLINK = 101,
    ADC = 200,
    ADC_UNIT_INIT = 210,
    ADC_UNIN_DEL = 220,
    ADC_CHANEL_CONFIG = 230,
    LEDC_CONFIGURE = 300,
    LEDC_RECONFIGURE_TIMER = 310,
    LEDC_RECONFIGURE_CHANNEL = 320,
    LEDC_SET_DUTY = 330,


} Commands;

void commander_configure_on_board_led(void);

void commander_blink(int32_t count);

int32_t commander_configure_of_LEDC(int32_t timers[], int32_t channels[]);

int32_t commander_reconfigure_LEDC_timer(int32_t timer, int32_t freq);

int32_t commander_reconfigure_LEDC_channel(int32_t timer, int32_t ch, int32_t gpio_num);

int32_t commander_set_duty_of_LEDC_channel(int32_t ch, int32_t val);