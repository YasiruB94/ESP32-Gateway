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

#define GPIO_HANDSHAKE 2
#define GPIO_MOSI 12
#define GPIO_MISO 13
#define GPIO_SCLK 14
#define GPIO_CS 15
#define BUFFER 100
static const char *TAG = "SPI_SLAVE";
spi_bus_config_t buscfg;
spi_slave_interface_config_t slvcfg;
gpio_config_t io_conf;
void my_post_setup_cb(spi_slave_transaction_t *trans)
{

    WRITE_PERI_REG(GPIO_OUT_W1TS_REG, (1 << GPIO_HANDSHAKE));
}
void my_post_trans_cb(spi_slave_transaction_t *trans)
{

    WRITE_PERI_REG(GPIO_OUT_W1TC_REG, (1 << GPIO_HANDSHAKE));
}
esp_err_t init_SPI(){
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
    uint8_t n = 0;
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
    while (1)
    {
        char sendbuf[BUFFER];
        uint8_t recvbuf[sizeof(Buffer_t)];
        Buffer_t *testbuf = malloc(sizeof(Buffer_t));
        if (testbuf == NULL)
        {
            ESP_LOGE(TAG, "Error in testbuf");
        }
        else
        {
            memset(testbuf, 0, sizeof(Buffer_t));
            memset(recvbuf, 0, sizeof(Buffer_t));
            sprintf(sendbuf, "ESP32 transmission # %u.", n);

            // Set up a transaction of 128 bytes to send/receive
            t.length = 128 * 8;
            t.tx_buffer = sendbuf;
            t.rx_buffer = recvbuf;
            /* This call enables the SPI slave interface to send/receive to the sendbuf and recvbuf. The transaction is
            initialized by the SPI master, however, so it will not actually happen until the master starts a hardware transaction
            by pulling CS low and pulsing the clock etc. In this specific example, we use the handshake line, pulled up by the
            .post_setup_cb callback that is called as soon as a transaction is ready, to let the master know it is free to transfer
            data.
            */
            // ret=spi_slave_transmit(HSPI_HOST, &t, portMAX_DELAY);
            ret = spi_slave_queue_trans(HSPI_HOST, &t, portMAX_DELAY);
            if (ret == ESP_OK)
            {
                memcpy(testbuf, recvbuf, sizeof(Buffer_t));
                ESP_LOGI(TAG, "Received(%u): testbuf->buffer: %u || testbuf->size: %d", n, (uint8_t)(testbuf->buffer), (uint16_t)(testbuf->size));
                
                n++;
            }
            else
            {
                ESP_LOGI(TAG, "error code: %s", esp_err_to_name(ret));
            }

            // spi_slave_transmit does not return until the master has done a transmission, so by here we have sent our data and
            // received data from the master. Print it.

            free(testbuf);
        }
    }
}