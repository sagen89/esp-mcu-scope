#include <stdint.h>

int32_t pwm_led_configure(int32_t timers[], int32_t channels[]);

int32_t pwm_led_reconfigure_timer(int32_t timer, int32_t freq);

int32_t pwm_led_reconfigure_channel(int32_t timer, int32_t ch, int32_t gpio_num);

int32_t pwm_led_set_duty_of_channel(int32_t ch, int32_t val);