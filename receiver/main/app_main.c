#include "inc/app_main.h"
#include "inc/gpio.h"
#include "inc/SPI_comm.h"
#include "inc/handshake.h"
#include "inc/action.h"
#include "inc/handle_commands.h"

static const char *TAG = "APP_MAIN";



void GW_process_received_data(void *pvParameters)
{
    bool print_recv_data = false;
    uint8_t receivedData[BUFFER];
    while (1)
    {
        memset(receivedData, 0, BUFFER);
        if (xQueueReceive(CN_message_queue, receivedData, portMAX_DELAY) == pdTRUE)
        {
            if (print_recv_data)
            {
                printf("Received new:");
                int i = 0;
                while (i < BUFFER)
                {
                    printf("%02X ", (int)receivedData[i]);
                    i++;
                }
                printf("\n");
            }
            Parse_1_Frame(receivedData, BUFFER);
        }
    }
}

void app_main()
{
    init_GPIO();
    //gpio_set_level(GW_ON, 1);
   // ESP_LOGI(TAG, "Starting...");
   // vTaskDelay(pdMS_TO_TICKS(4000));
   // ESP_LOGI(TAG, "resetting GW");
    Reset_GW();

    esp_err_t ret;
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    ret = init_SPI();
    xTaskCreate(GW_Trancieve_Data, "GW_Trancieve_Data", 4096, NULL, 10, NULL);
    xTaskCreate(GW_process_received_data, "GW_process_received_data", 4096, NULL, 10, NULL);
    xTaskCreate(dum_pin_function, "dum_pin_function", 4096, NULL, 10, NULL);
}

void dum_pin_function(void *pvParameters)
{
    bool state = false;
    while (1)
    {

        int level = gpio_get_level(DUM_PIN);

        if (level)
        {
            state = !state;
            ESP_LOGW(TAG, "Dum pin HIGH. adding message to Tx queue. state: %d", state);
            if (state)
            {
                Send_Action_Msg_On_Off(1, 16, CNGW_ACTION_CMD_On_Full, 2047);
            }
            else
            {
                Send_Action_Msg_On_Off(1, 16, CNGW_ACTION_CMD_Off, 0);
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        vTaskDelay(pdMS_TO_TICKS(10));

    }
}
