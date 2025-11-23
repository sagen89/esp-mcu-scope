#include <stdio.h>
#include "hardware_serial.h"
#include "converter.h"
#include "commander.h"

int32_t command;
int32_t transmittedValue ;

uint16_t valueOfChannel1;
uint8_t ubytesForValueOfChannel1[sizeof(uint16_t)];

int write_int32(int32_t value) {
  uint8_t size = sizeof(int32_t);
  uint8_t bytes[size];
  converter_int32_to_bytes(value, bytes);
  return serial_write_bytes(bytes, size);
}

void app_main(void) {
    serial_begin();
    commander_configure_on_board_led();
    
    while (1) {
        
        if (serial_available() > 7) {
            uint8_t cmdBuffer[4];
            uint8_t transmittedValueBuffer[4];

            serial_read_bytes(cmdBuffer, 4);
            serial_read_bytes(transmittedValueBuffer, 4);

            command = converter_bytes_to_int32(cmdBuffer);
            transmittedValue = converter_bytes_to_int32(transmittedValueBuffer);

            switch (command) {
                case(BLINK):
                    commander_blink(transmittedValue);
                    serial_flush();
                    write_int32(command);
                    write_int32(transmittedValue);
                    break;
                case(SET_UP_CHANNEL_ONE):
                    serial_flush();
                    break;
                default: serial_flush(); break;
            }
            
        }
        // Read data from the UART
        // int len = uart_read_bytes(uart_port_num, data, (BUF_SIZE - 1), 20 / portTICK_PERIOD_MS);
        
        
        // Write data back to the UART
        // uart_write_bytes(uart_port_num, (const char *) data, len);
        // if (len) {
        //     data[len] = '\0';
        //     ESP_LOGI(TAG, "Recv str: %s", (char *) data);
        // }
    }
}