#include <stdint.h>

extern int global_rx_buff_size;

void serial_begin(void);

uint16_t serial_available();

void serial_flush();

int serial_read_bytes(uint8_t *buffer, int length);

int serial_write_bytes(const uint8_t *buffer, int size);

