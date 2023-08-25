#include "inc/app_main.h"
#include "inc/gpio.h"
#include "inc/SPI_comm.h"
#include "inc/handshake.h"
#include "inc/action.h"
#include "inc/query.h"
#include "inc/control.h"
#include "inc/handle_commands.h"
#include "inc/I2C_comm.h"
#include "include/atecc508a.h"

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
    ret = atecc508a_init();
    xTaskCreate(GW_Trancieve_Data, "GW_Trancieve_Data", 4096, NULL, 10, NULL);
    xTaskCreate(GW_process_received_data, "GW_process_received_data", 4096, NULL, 10, NULL);
    xTaskCreate(dum_pin_function, "dum_pin_function", 4096, NULL, 10, NULL);
    //xTaskCreate(dum_I2C_function, "dum_I2C_function", 4096, NULL, 10, NULL);
    //xTaskCreate(I2C_read, "I2C_read", 4096, NULL, 10, NULL);
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
                //Send_Action_Msg_broadcast_On_Off(1, 16, CNGW_ACTION_CMD_On_Full, 2047);
                //query_all_channel_info();
                //send_control_message(CNGW_CONTROL_CMD_Log, CNGW_LOG_INFO, CNGW_SOURCE_MCU_CN);
            }
            else
            {
                //Send_Action_Msg_broadcast_On_Off(1, 16, CNGW_ACTION_CMD_Off, 0);
                //query_all_channel_info();
                //send_control_message(CNGW_CONTROL_CMD_Log, CNGW_LOG_DISABLE, CNGW_SOURCE_MCU_CN);
            }
            vTaskDelay(pdMS_TO_TICKS(1000));
        }
        vTaskDelay(pdMS_TO_TICKS(10));

    }
}

void dum_I2C_function(void *pvParameters)
{   
    ESP_LOGI(TAG, "dum_I2C_function");
    bool state = false;
    while (1)
    {    // Write data to the device

        uint8_t random[35] = {0};
        atecc508a_random(random,0);
        printf("####atecc508a_random: ");
        for (int i = 0; i < 32; i++)
        {
            printf("%u ", random[i]);
        }
        printf("\n");
        vTaskDelay(pdMS_TO_TICKS(1000));

    }
}
