idf_component_register(SRCS "main.c"
                    INCLUDE_DIRS "."
                    REQUIRES hardware_serial converter commander)