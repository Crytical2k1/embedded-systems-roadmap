#ifndef DATA_PROCESSOR_H
#define DATA_PROCESSOR_H

//refactor constants
#define VD_RESISTOR 5100U //5.1kOhm known resistance of voltage divider
#define V_IN 3300 //3.3V  vref for voltage divider
#define V_max 2450 //2.45V value for adc raw to voltage
#define D_max 4095 // value for adc raw to voltage
//NTC constants
#define NTC_T0 298.15f // reference temperature 25ºC
#define NTC_B 3950.0f // beta value
#define NTC_R25 10000.0f //resistance at 25ºC, 10kOhm
#define KELVIN_CELSIUS 273.15f
//Photoresistor constants
#define PHOTO_R10 30000.0f // resistance at 10Lux, 30kOhm (dont know)
#define PHOTO_GAMMA (float)0.7 // gamma value

void raw_proccesing_task(void *pvParameters);

#endif