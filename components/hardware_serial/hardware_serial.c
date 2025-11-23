#include "hardware_serial.h"
// #include <stddef.h>
#include <stdint.h>
#include "sdkconfig.h"
#include "driver/uart.h"

#define UART_TXD (CONFIG_UART_TXD)
#define UART_RXD (CONFIG_UART_RXD)
#define UART_RTS (UART_PIN_NO_CHANGE)
#define UART_CTS (UART_PIN_NO_CHANGE)

#define UART_PORT_NUM      (CONFIG_UART_PORT_NUM)
#define UART_BAUD_RATE     (CONFIG_UART_BAUD_RATE)

#define BUF_SIZE (1024)
#define TIMEOUT_MS (10000)

uart_config_t uart_config = {
        .baud_rate = UART_BAUD_RATE,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
        .source_clk = UART_SCLK_DEFAULT,
};

void serial_begin(void) {
    int intr_alloc_flags = 0;
    uart_driver_install(UART_PORT_NUM, BUF_SIZE * 2, 0, 0, NULL, intr_alloc_flags);
    uart_param_config(UART_PORT_NUM, &uart_config);
    uart_set_pin(UART_PORT_NUM, UART_TXD, UART_RXD, UART_RTS, UART_CTS);
}

uint16_t serial_available() {
    uint16_t length = 0;
    uart_get_buffered_data_len(UART_PORT_NUM, (size_t*)&length);
    return length;
}

void serial_flush() {
    uart_flush_input(UART_PORT_NUM);
}

int serial_read_bytes(uint8_t *buffer, int length) {
    return uart_read_bytes(UART_PORT_NUM, buffer, (size_t)length, pdMS_TO_TICKS(TIMEOUT_MS));
}

int serial_write_bytes(const uint8_t *buffer, int size) {
    return uart_write_bytes(UART_PORT_NUM, buffer, (size_t)size);
}



