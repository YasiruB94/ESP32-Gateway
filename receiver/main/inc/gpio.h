#ifndef GPIO_H
#define GPIO_H
#include "app_main.h"
#define BDATA_READY 27
#define GW_EX1 33
#define GW_ON 26
#define INDICATOR_LED 2

esp_err_t init_GPIO();
esp_err_t Reset_GW();
esp_err_t Signal_GW_Of_Sending_Data();
esp_err_t Signal_GW_Of_Receiving_Data();
#endif