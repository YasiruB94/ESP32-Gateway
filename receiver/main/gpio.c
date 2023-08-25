#include "inc/gpio.h"

esp_err_t init_GPIO()
{
    gpio_config_t io_conf;
    io_conf.pin_bit_mask = (1ULL << BDATA_READY | 1ULL << GW_ON | 1ULL << GW_EX1 | 1ULL << INDICATOR_LED);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);

    // Configure the GPIO pin as input
    gpio_config_t io_config_in;
    io_config_in.pin_bit_mask = 1ULL << DUM_PIN;
    io_config_in.mode = GPIO_MODE_INPUT;
    io_config_in.pull_up_en = GPIO_PULLUP_DISABLE;
    io_config_in.pull_down_en = GPIO_PULLDOWN_ENABLE;
    io_config_in.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_config_in);

    return ESP_OK;
}

esp_err_t Reset_GW()
{
    gpio_set_level(BDATA_READY, 0);
    gpio_set_level(GW_ON, 1);
    vTaskDelay(pdMS_TO_TICKS(100));
    gpio_set_level(GW_ON, 0);

    return ESP_OK;
}

esp_err_t Signal_GW_Of_Sending_Data()
{
    gpio_set_level(GW_ON, 1);
    vTaskDelay(pdMS_TO_TICKS(1));
    gpio_set_level(GW_ON, 0);

    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(GW_EX1, 0);
    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(GW_EX1, 1);

    return ESP_OK;
}
esp_err_t Signal_GW_Of_Receiving_Data()
{
   // gpio_set_level(GW_ON, 1);
   // gpio_set_level(BDATA_READY, 0);

   // vTaskDelay(pdMS_TO_TICKS(10));

   // gpio_set_level(GW_ON, 0);
  //  gpio_set_level(BDATA_READY, 0);

 //   vTaskDelay(pdMS_TO_TICKS(10));
  //  gpio_set_level(GW_EX1, 0);


  //  vTaskDelay(pdMS_TO_TICKS(10));
  //  gpio_set_level(GW_EX1, 1);

    return ESP_OK;
}
/*
esp_err_t Signal_GW_Of_Sending_Data(){
    gpio_set_level(GW_ON, 1);
    gpio_set_level(BDATA_READY, 0);

    vTaskDelay(pdMS_TO_TICKS(10));

    gpio_set_level(GW_ON, 0);
    gpio_set_level(BDATA_READY, 0);

    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(GW_EX1, 0);
    //gpio_set_level(BDATA_READY, 1);

    vTaskDelay(pdMS_TO_TICKS(10));
    gpio_set_level(GW_EX1, 1);
    //gpio_set_level(BDATA_READY, 0);
    return ESP_OK;
}
*/