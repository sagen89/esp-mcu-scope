#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "sdkconfig.h"
#include "hardware_serial.h"
#include "converter.h"
#include "commander.h"
#include "esp_log.h"

#define SIZE_OF_ELEMENTS_PDU (CONFIG_SIZE_OF_ELEMENTS_PDU)
#define LEDC_TIMER_NUM (CONFIG_LEDC_TIMER_NUM)
#define LEDC_CHANNEL_NUM (CONFIG_LEDC_CHANNEL_NUM)

int32_t command;
int32_t transmittedValue ;

const char *TAG = "Main: ";

int write_int32(int32_t value);

// Function to convert a byte array to a hexadecimal string
char* bytes_to_string(const uint8_t *data, size_t length);

void app_main(void) {
    serial_begin();
    commander_configure_on_board_led();
    // commander_add_interrupt_of_LEDC_module();

    // uint8_t bytes_of_command_and_values[1 + NUMBER_OF_ELEMENTS][__max(SIZE_OF_COMMAND, SIZE_OF_ELEMENT)];

    uint8_t *protocol_data = (uint8_t *) malloc(global_rx_buff_size);
    // int32_t *timers = (int32_t *) malloc(LEDC_TIMER_NUM * 3*4);
    // int32_t *channels = (int32_t *) malloc(LEDC_CHANNEL_NUM * 3*4);

    while (1) {
        int length = serial_read_bytes(protocol_data, global_rx_buff_size - 1);

        if (length) {
            ESP_LOGI(TAG, "Received bytes %d from UART%d: %s", 
                length, CONFIG_UART_PORT_NUM, bytes_to_string(protocol_data, length));           
        }

        if (length < SIZE_OF_ELEMENTS_PDU * 2) {
            continue;
        }


        uint8_t rows = length / SIZE_OF_ELEMENTS_PDU;
        uint8_t bytes_of_command_and_values[rows][SIZE_OF_ELEMENTS_PDU];
        int32_t command_and_values[rows][1];
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
                write_int32(command_and_values[0][0]);
                write_int32(command_and_values[1][0]);
                break; 
            case(ADC_CONFIGURE_CONTINUOUS):
                ESP_LOGI(TAG, "ADC_CONFIGURE_CONTINUOUS command: channel num: %d, sample freq: %d, count of samples by ch: %d", command_and_values[1][0], command_and_values[2][0], command_and_values[3][0]);
               
                write_int32(command_and_values[0][0]);
                write_int32(commander_configure_of_ADC_continuous(command_and_values[1][0], command_and_values[2][0], command_and_values[3][0]));
                break;
            case(ADC_START_CONTINUOUS):
                ESP_LOGI(TAG, "ADC_START_CONTINUOUS command");
    
                write_int32(command_and_values[0][0]);
                write_int32(commander_start_ADC_continuous());
                break;
            case(ADC_READ_RAW_DATA_CONTINUOUS):
                ESP_LOGI(TAG, "ADC_READ_RAW_DATA_CONTINUOUS command: %d", command_and_values[1][0]);
                
                int32_t *raw_data = (int32_t *) malloc(command_and_values[1][0] * sizeof(int32_t));
                memset(raw_data, 0xFF, command_and_values[1][0] * sizeof(int32_t));

                commander_read_raw_data_ADC_continuous(raw_data);

                for (int32_t i = 0; i < command_and_values[1][0]; i++) {
                    // ESP_LOGI(TAG, "ADC_READ_RAW_DATA_CONTINUOUS: %d", raw_data[i]);
                    write_int32(raw_data[i]);
                }

                free(raw_data);
                raw_data = NULL;
                break;
            case(ADC_STOP_CONTINUOUS):
                ESP_LOGI(TAG, "ADC_STOP_CONTINUOUS command");
                
                write_int32(command_and_values[0][0]);
                write_int32(commander_stop_ADC_continuous());
                break;
            case(ADC_DEINITIALIZE_CONTINUOUS):
                ESP_LOGI(TAG, "ADC_DEINITIALIZE_CONTINUOUS command");
                
                write_int32(command_and_values[0][0]);
                write_int32(commander_deinitialize_ADC_continuous());
                break;
            case(LEDC_CONFIGURE):
                ESP_LOGI(TAG, "LEDC_CONFIGURE command: %d", command_and_values[1][0]);

                int32_t *timers = (int32_t *) malloc(LEDC_TIMER_NUM * 3 * sizeof(int32_t));
                memset(timers, 0xFF, LEDC_TIMER_NUM * 3 * sizeof(int32_t));
                int32_t *channels = (int32_t *) malloc(LEDC_CHANNEL_NUM * 3 * sizeof(int32_t));
                memset(channels, 0xFF, LEDC_CHANNEL_NUM * 3 * sizeof(int32_t));
                commander_configure_of_PWM(timers, channels);

                write_int32(command_and_values[0][0]);

                for (uint8_t i = 0; i < LEDC_TIMER_NUM * 3; i++) {
                    ESP_LOGI(TAG, "LEDC_TIMER: %d", timers[i]);
                    write_int32(timers[i]);
                }
                for (uint8_t i = 0; i < LEDC_CHANNEL_NUM * 3; i++) {
                    ESP_LOGI(TAG, "LEDC_CHANNEL: %d", channels[i]);
                    write_int32(channels[i]);
                }
                
                free(timers);
                timers = NULL;
                free(channels);
                channels = NULL;
                break;
            case(LEDC_RECONFIGURE_TIMER):
                ESP_LOGI(TAG, "LEDC_RECONFIGURE_TIMER command: %d, %d", command_and_values[1][0], command_and_values[2][0]);
                write_int32(command_and_values[0][0]);
                write_int32(commander_reconfigure_of_PWM_timer(command_and_values[1][0], command_and_values[2][0]));
                break;
            case(LEDC_RECONFIGURE_CHANNEL):
                ESP_LOGI(TAG, "LEDC_RECONFIGURE_CHANNEL command:  %d, %d, %d", command_and_values[1][0], command_and_values[2][0], command_and_values[3][0]);
                write_int32(command_and_values[0][0]);
                write_int32(commander_reconfigure_of_PWM_channel(command_and_values[1][0], command_and_values[2][0], command_and_values[3][0]));
                break;
            case(LEDC_SET_DUTY):
                ESP_LOGI(TAG, "LEDC_SET_DUTY command: %d, %d", command_and_values[1][0], command_and_values[2][0]);
                write_int32(command_and_values[0][0]);
                write_int32(commander_set_duty_of_PWM_channel(command_and_values[1][0], command_and_values[2][0]));
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