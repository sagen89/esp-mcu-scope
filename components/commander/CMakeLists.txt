idf_component_register(SRCS "commander.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_driver_gpio analog_digital_converter pwm_led)
