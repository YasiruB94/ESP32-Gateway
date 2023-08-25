#ifndef SPI_COMM_H
#define SPI_COMM_H
#include "app_main.h"
#include "gpio.h"
#include "handshake.h"

#define GPIO_MOSI 12
#define GPIO_MISO 13
#define GPIO_SCLK 14
#define GPIO_CS 15
#define BUFFER 200 //140
#define QUEUE_LENGTH 10
extern QueueHandle_t GW_response_queue; 
extern QueueHandle_t CN_message_queue;
esp_err_t init_SPI();
void GW_Send_Data(void *pvParameters);
void GW_Receive_Data(void *pvParameters);
void GW_Trancieve_Data(void *pvParameters);
esp_err_t is_all_zeros(const uint8_t *array, size_t size);
esp_err_t consume_GW_message(uint8_t *message);

#endif