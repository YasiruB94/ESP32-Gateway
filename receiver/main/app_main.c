#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#include "lwip/sockets.h"
#include "lwip/dns.h"
#include "lwip/netdb.h"
#include "lwip/igmp.h"

#include "esp_wifi.h"
#include "esp_system.h"
#include "esp_event.h"
#include "esp_event_loop.h"
#include "nvs_flash.h"
#include "soc/rtc_cntl_reg.h"
#include "rom/cache.h"
#include "driver/spi_slave.h"
#include "esp_log.h"
#include "esp_spi_flash.h"
#include "cn_structs.h"
#include "cngw_action_structs.h"

#define GPIO_MOSI 12
#define GPIO_MISO 13
#define GPIO_SCLK 14
#define GPIO_CS 15
#define BUFFER 90

spi_bus_config_t buscfg;
spi_slave_interface_config_t slvcfg;
CCP_TX_FRAMES_t lmcontrol_res;
CNGW_Action_Frame_t actionFrame;
const CNGW_Message_Header_t message_header_instance = {
    .command_type = CNGW_HEADER_TYPE_Action_Commmand,
    .data_size = 30,
    .crc = 30};
esp_err_t init_SPI()
{
    // Configuration for the SPI bus
    spi_bus_config_t buscfg = {
        .mosi_io_num = GPIO_MOSI,
        .miso_io_num = GPIO_MISO,
        .sclk_io_num = GPIO_SCLK};

    // Configuration for the SPI slave interface
    spi_slave_interface_config_t slvcfg = {
        .mode = 0,
        .spics_io_num = GPIO_CS,
        .queue_size = 1, // 3
        .flags = 0};

    // Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);

    // Initialize SPI slave interface
    return spi_slave_initialize(HSPI_HOST, &buscfg, &slvcfg, 1);
}

esp_err_t dummy_Action_init()
{
    actionFrame.header.command_type = CNGW_HEADER_TYPE_Action_Commmand;
    actionFrame.header.data_size = 30; //
    actionFrame.header.crc = 30;       //

    actionFrame.message.command = CNGW_ACTION_CMD_On_Full;
    actionFrame.message.address.target_cabinet = 5;
    actionFrame.message.address.address_type = CNGW_ADDRESS_TYPE_Cense_Channel;
    actionFrame.message.address.target_address = 21;

    actionFrame.message.action_parameters.fade_unit = CNGW_FADE_UNIT_Milliseconds;
    actionFrame.message.action_parameters.fade_time = 100;
    actionFrame.message.action_parameters.light_level = 512;
    actionFrame.message.action_parameters.reserved = 0;
    actionFrame.message.scene_parameters.reserved = 64;
    actionFrame.message.crc = 30; //

    lmcontrol_res.action = &actionFrame;
    // const CNGW_Message_Header_t *const temp_header_ptr = &message_header_instance;
    // lmcontrol_res.raw_data = "";//
    // CNGW_Message_Header_t temp_header;
    // memcpy(&temp_header, &message_header_instance, sizeof(CNGW_Message_Header_t));
    // lmcontrol_res.generic_header =  temp_header_ptr;
    return ESP_OK;
}

void SPI_RxTx()
{   
    //create lmcontrol_res which contains the action command.
    esp_err_t ret = dummy_Action_init();
    assert(ret == ESP_OK);
    //create a sendbuf array and copy lmcontrol_res to it.
    uint8_t sendbuf[sizeof(CCP_TX_FRAMES_t)];
    memset(sendbuf, 0, sizeof(CCP_TX_FRAMES_t));
    memcpy(sendbuf, &lmcontrol_res, sizeof(CCP_TX_FRAMES_t));
    //check the content of sendbuf
    printf("sendbuf: ");
    int i = 0;
    while (i < sizeof(sendbuf))
    {
        printf("%02X", (int)sendbuf[i]);
        i++;
    }
    printf("\n");
    printf("==================\n");
    //define slave transaction and set memory to zero
    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));
    //create a recvbuf array and set memory to zero
    uint8_t recvbuf[BUFFER];
    memset(recvbuf, 0, sizeof(recvbuf));
    //set the slave transaction tx_buffer and rx_buffer to the created buffers above
    t.length = 128 * 8;
    // t.tx_buffer = NULL;
    t.tx_buffer = sendbuf;
    t.rx_buffer = recvbuf;

    while (1)
    {   //after each spi transmit, reset the recvbuf memory
        memset(recvbuf, 0, sizeof(recvbuf));
        //perform a SPI transaction. waits indefinetly until theres a returned item.
        ret = spi_slave_transmit(HSPI_HOST, &t, portMAX_DELAY);
        //checks if there was a proper SPI transaction and that the data is not the echo of sent data
        if (ret == ESP_OK && memcmp(sendbuf, recvbuf, sizeof(sendbuf)))
        {
            CCP_HANDSHAKE_CN_MESSAGES_t cn_messages = {0};
            size_t bytes = 0;
            cn_messages.command = *((CNGW_Handshake_Command *)recvbuf);
            switch (cn_messages.command)
            {
            case CNGW_Handshake_CMD_CN1:
                cn_messages.cn1_message = *((CNGW_Handshake_CN1_t *)recvbuf);
                bytes = sizeof(cn_messages.cn1_message);
                printf("case 1 sizeof(bytes): %d\n", bytes);
                printf("command: %d\n", cn_messages.cn1_message.command);
                printf("mainboard_serial: %u:%u:%u:%u:%u:%u:%u:%u:%u\n", cn_messages.cn1_message.mainboard_serial[0], cn_messages.cn1_message.mainboard_serial[1], cn_messages.cn1_message.mainboard_serial[2], cn_messages.cn1_message.mainboard_serial[3], cn_messages.cn1_message.mainboard_serial[4], cn_messages.cn1_message.mainboard_serial[5], cn_messages.cn1_message.mainboard_serial[6], cn_messages.cn1_message.mainboard_serial[7], cn_messages.cn1_message.mainboard_serial[8]);
                printf("cabinet_number: %d\n", cn_messages.cn1_message.cabinet_number);
                printf("challenge size: %u\n", sizeof(cn_messages.cn1_message.challenge));
                printf("hmac size: %u\n", sizeof(cn_messages.cn1_message.hmac));
                break;
            case CNGW_Handshake_CMD_CN2:
                cn_messages.cn2_message = *((CNGW_Handshake_CN2_t *)recvbuf);
                bytes = sizeof(cn_messages.cn2_message);
                printf("case 2 sizeof(bytes): %d\n", bytes);
                printf("command: %d\n", cn_messages.cn2_message.command);
                printf("status: %d\n", cn_messages.cn2_message.status);
                printf("hmac size: %d\n", sizeof(cn_messages.cn2_message.hmac));

                break;
            default:
                printf("Received: ");
                int i = 0;
                while (i < sizeof(recvbuf))
                {
                    printf("%02X", (int)recvbuf[i]);
                    i++;
                }
                printf("\n");
                break;
            }
            printf("\n");
        }
        //either the spi transaction did not happen well, or the received data is the echo of sent data
        else{
            if(ret != ESP_OK){
                printf("Error in SPI transaction. error code: %s\n", esp_err_to_name(ret));
            }
        }
    }
}

void app_main()
{
    esp_err_t ret;
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    ret = init_SPI();
    assert(ret == ESP_OK);
    xTaskCreatePinnedToCore(SPI_RxTx, "SPI_RxTx", 8 * 1024, NULL, 5, NULL, 0);
}
