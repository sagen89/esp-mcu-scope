#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "hardware_serial.h"
#include "converter.h"
#include "commander.h"
#include "esp_log.h"

#define SIZE_OF_ELEMENTS_PDU (CONFIG_SIZE_OF_ELEMENTS_PDU)


int32_t command;
int32_t transmittedValue ;

const char *TAG = "Main: ";

int write_int32(int32_t value);

// Function to convert a byte array to a hexadecimal string
char* bytes_to_string(const uint8_t *data, size_t length);

void commander_task(void *arg) {
    serial_begin();
    commander_configure_on_board_led();
    // uint8_t data[8];
    uint8_t data[8];
    // uint8_t *cmdBuffer = (uint8_t *) malloc(4);;
    // uint8_t *transmittedValueBuffer = (uint8_t *) malloc(4);;
    while (1) {
        
        
        int length = serial_read_bytes(data, 8);
        serial_write_bytes(data, length);

        // int len = serial_read_bytes(data, 8);
        // serial_write_bytes(data, 8);

        // if (serial_available() > 7) {
            

            // serial_read_bytes(cmdBuffer, 4);
            // serial_read_bytes(transmittedValueBuffer, 4);
            // serial_flush();

            // command = converter_bytes_to_int32(cmdBuffer);
            // transmittedValue = converter_bytes_to_int32(transmittedValueBuffer);
            // uart_write_bytes(2, (const char *) cmdBuffer, 4);
            // uart_write_bytes(2, (const char *) transmittedValueBuffer, 4);
            // write_int32(command);
            // write_int32(transmittedValue);
            // switch (command) {
            //     case(BLINK):
                    
            //         commander_blink(transmittedValue);
            //         break;
            //     case(SET_UP_CHANNEL_ONE):
            //         serial_flush();
            //         break;
            //     default: serial_flush(); break;
            // }
            
        // }
        // Read data from the UART
        // int len = uart_read_bytes(uart_port_num, data, (RX_BUFFER_SIZE - 1), 20 / portTICK_PERIOD_MS);
        
        
        // Write data back to the UART
        // uart_write_bytes(uart_port_num, (const char *) data, len);
        // if (len) {
        //     data[len] = '\0';
        //     ESP_LOGI(TAG, "Recv str: %s", (char *) data);
        // }
    }
}

void app_main(void) {
    serial_begin();
    commander_configure_on_board_led();
    
    // uint8_t bytes_of_command_and_values[1 + NUMBER_OF_ELEMENTS][__max(SIZE_OF_COMMAND, SIZE_OF_ELEMENT)];

    uint8_t *protocol_data = (uint8_t *) malloc(global_rx_buff_size);

    while (1) {
        int length = serial_read_bytes(protocol_data, global_rx_buff_size - 1);
        serial_write_bytes(protocol_data, length);


        if (length) {
            ESP_LOGI(TAG, "Received bytes %d from UART%d: %s", 
                length, CONFIG_UART_PORT_NUM, bytes_to_string(protocol_data, length));           
        }

        if (length < SIZE_OF_ELEMENTS_PDU * 2) {
            continue;
        }

        uint8_t rows = length / SIZE_OF_ELEMENTS_PDU;
        uint8_t bytes_of_command_and_values[rows][SIZE_OF_ELEMENTS_PDU];
        uint8_t command_and_values[rows][1];
        for (uint8_t i = 0; i < rows; i++) {
            memcpy(&bytes_of_command_and_values[i], &protocol_data[i * SIZE_OF_ELEMENTS_PDU], SIZE_OF_ELEMENTS_PDU);
            command_and_values[i][0] = converter_bytes_to_int32(bytes_of_command_and_values[i]);
            ESP_LOGI(TAG, "converts bytes %s to int32: %d",
                 bytes_to_string(bytes_of_command_and_values[i], SIZE_OF_ELEMENTS_PDU), command_and_values[i][0]);
        }

        switch (command_and_values[0][0]) {
            case(BLINK):
                ESP_LOGI(TAG, "BLINK command: %d", command_and_values[1][0]);
                commander_blink(command_and_values[1][0]);
                break;
            case(SET_UP_CHANNEL_ONE):
                serial_flush();
                break;
            default: serial_flush(); break;
        }
    }
}

char* bytes_to_string(const uint8_t *data, size_t length) {
    char buffer_for_byte[] = "255";// UINT8_T_MAX = 255;
    char term[] = "|";
    char delimiter[] = ", ";
    char *str = (char *) malloc(strlen(term) * 2 + strlen(buffer_for_byte) * length + strlen(delimiter) * (length - 1) + 1);

    strcpy(str, term);
    for (uint8_t i = 0; i < length; i++) {
        itoa(data[i], buffer_for_byte, 10);
        strcat(str, buffer_for_byte);
        if (i != (length - 1)) {
            strcat(str, delimiter);
        }
    }
   
    return strcat(str, term);
}


int write_int32(int32_t value) {
    uint8_t size = sizeof(int32_t);
    uint8_t bytes[size];
    converter_int32_to_bytes(value, bytes);
    return serial_write_bytes(bytes, size);
}