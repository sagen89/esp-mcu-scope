idf_component_register(SRCS "commander.c"
                    INCLUDE_DIRS "include"
                    REQUIRES esp_driver_gpio esp_adc esp_driver_ledc esp_driver_gpio)
