idf_component_register(
    SRCS "ntc_thermistor.c" "cds_photoresistor.c" "sensor_manager.c" "system_events.c"
    INCLUDE_DIRS "include"
    REQUIRES driver esp_adc
)