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

#define GPIO_HANDSHAKE 2
#define GPIO_MOSI 12
#define GPIO_MISO 13
#define GPIO_SCLK 14
#define GPIO_CS 15
#define BUFFER 144
static const char *TAG = "SPI_SLAVE";
spi_bus_config_t buscfg;
spi_slave_interface_config_t slvcfg;
gpio_config_t io_conf;
CCP_TX_FRAMES_t lmcontrol_res;// = malloc(sizeof(CCP_TX_FRAMES_t));

//void CCP_HANDLER_Rx_Packet(const CCP_PACKET_t *const rx_packet, uint8_t *const is_bd_query_received);
//esp_err_t create_dummy_action(struct CCP_TX_Frames *action);
void my_post_setup_cb(spi_slave_transaction_t *trans)
{

    WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (1 << GPIO_HANDSHAKE));
}
void my_post_trans_cb(spi_slave_transaction_t *trans)
{

    WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (1 << GPIO_HANDSHAKE));
}
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
        .queue_size = 3,
        .flags = 0,
        .post_setup_cb = my_post_setup_cb,
        .post_trans_cb = my_post_trans_cb};

    // Configuration for the handshake line
    gpio_config_t io_conf = {
        .intr_type = GPIO_INTR_DISABLE,
        .mode = GPIO_MODE_OUTPUT,
        .pin_bit_mask = (1 << GPIO_HANDSHAKE)};

    // Configure handshake line as output
    gpio_config(&io_conf);
    // Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);

    // Initialize SPI slave interface
    return spi_slave_initialize(HSPI_HOST, &buscfg, &slvcfg, 1);
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
    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));
    ret = init_SPI();
    assert(ret == ESP_OK);

    //lmcontrol_res.action->header.command_type = CNGW_HEADER_TYPE_Action_Commmand;
    // lmcontrol_res.action->header.data_size = 0;  // Set appropriate data size
     lmcontrol_res.action->header.crc = 5;
    //ESP_LOGI(TAG, "lmcontrol_res.generic_header->crc: %d",sizeof(lmcontrol_res));
   


    while (1)
    {
        uint8_t recvbuf[BUFFER];
        uint8_t sendbuf[BUFFER];
        memset(recvbuf, 0, sizeof(recvbuf));
        memset(sendbuf, 0, sizeof(sendbuf));

        // Set up a transaction of 128 bytes to send/receive
        t.length = 128 * 8; // sizeof(Buffer_t); // 128 * 8;
        t.tx_buffer = NULL; // sendbuf;

        t.rx_buffer = recvbuf;

        ret = spi_slave_transmit(HSPI_HOST, &t, portMAX_DELAY);
        if (ret == ESP_OK)
        {
             /*
            printf("Received: ");
            int i = 0;
            while (i < sizeof(recvbuf))
            {
                printf("%02X", (int)recvbuf[i]);
                i++;
            }
            printf("\n");
            */

            CCP_HANDSHAKE_CN_MESSAGES_t cn_messages = {0}; /*Must be cleared to 0*/
            //CCP_PACKET_t            packet          = { 0 };
            size_t bytes = 0;
            //CCP_HANDLER_Rx_Packet(&packet, 1);







            cn_messages.command = *((CNGW_Handshake_Command *)recvbuf);
            switch (cn_messages.command)
            {
            case CNGW_Handshake_CMD_CN1:
                //cn_messages.cn1_message = *((CNGW_Handshake_CN1_t *)buffer);
                //bytes = sizeof(cn_messages.cn1_message);
                printf("case 1\n");
                break;
            case CNGW_Handshake_CMD_CN2:
                cn_messages.cn2_message = *((CNGW_Handshake_CN2_t *)recvbuf);
                bytes = sizeof(cn_messages.cn2_message);
                printf("case 2 sizeof(bytes): %d\n",bytes);
                printf("cn_messages.cn2_message.command: %d\n", cn_messages.cn2_message.command);
                printf("cn_messages.cn2_message.status: %d\n", cn_messages.cn2_message.status);
                printf("cn_messages.cn2_message.hmac size: %d\n", sizeof(cn_messages.cn2_message.hmac));

                break;
            default:
                //printf("default\n");
                break;
            }
            // CCP_PACKET_t packet = { 0 };
            // xQueueReceive(&recvbuf, &packet, 0);
        }
    }
}

//void CCP_HANDLER_Rx_Packet(const CCP_PACKET_t *const rx_packet, uint8_t *const is_bd_query_received)
//{
    /*
    ASSERT_DEBUG_MODE( NULL != rx_packet );
    ASSERT_DEBUG_MODE( NULL != is_bd_query_received );


    if( NULL != rx_packet->start && NULL != rx_packet->end )
    {
        ASSERTF(rx_packet->start < rx_packet->end, "CCP_HANDLER_Rx_Packet(): Start >= End. The ISRs did this");

        *is_bd_query_received = 0;

        CCP_PACKET_t unparsed_frames = *rx_packet;

        while( NULL != unparsed_frames.start )
        {
            unparsed_frames.start = Parse_1_Frame(&unparsed_frames, is_bd_query_received);
            ASSERT_DEBUG_MODE(rx_packet->end == unparsed_frames.end); 
        }
    }
    */
//}


