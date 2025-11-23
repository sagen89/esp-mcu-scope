#include <stdint.h>

typedef enum {
    BLINK = 101,
    SET_UP_CHANNEL_ONE = 200,
} Commands;

void commander_configure_on_board_led(void);

void commander_blink(int32_t count);
