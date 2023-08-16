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
#include "cngw_handshake_structs.h"
#include "ccp_util.c"

#define GPIO_MOSI 12
#define GPIO_MISO 13
#define GPIO_SCLK 14
#define GPIO_CS 15
#define BDATA_READY 27
#define GW_ON 26
#define BUFFER 90
static const char *TAG = "ESP_SPI";

SemaphoreHandle_t sendBufferMutex;
spi_bus_config_t buscfg;
spi_slave_interface_config_t slvcfg;
uint8_t *sendBuffer;
size_t sendBufferSize = 0;
void analyze_CNGW_Handshake_CN1_t_data(const uint8_t *recvbuf, int size, bool print_data);
void analyze_CNGW_Handshake_CN2_t_data(const uint8_t *recvbuf, int size, bool print_data);
void prepare_CN_handshake_01_feedback(CNGW_Handshake_CN1_Frame_t *cn1);
void prepare_CN_handshake_02_feedback(CNGW_Handshake_CN2_Frame_t *cn2);
void CCP_UTIL_Get_Message_Header(CNGW_Message_Header_t *const header, CNGW_Header_Type command, const size_t data_size);
const CNGW_Firmware_Version_t *GWV_Get_Firmware(void);
const CNGW_Firmware_Version_t *GWV_Get_Bootloader_Firmware(void);
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
    // create lmcontrol_res which contains the action command.
    CCP_TX_FRAMES_t lmcontrol_res;
    CNGW_Action_Frame_t actionFrame;
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
    /*
    const CNGW_Message_Header_t message_header_instance = {
    .command_type = CNGW_HEADER_TYPE_Action_Commmand,
    .data_size = 30,
    .crc = 30};
    */
    // const CNGW_Message_Header_t *const temp_header_ptr = &message_header_instance;
    // lmcontrol_res.raw_data = "";//
    // CNGW_Message_Header_t temp_header;
    // memcpy(&temp_header, &message_header_instance, sizeof(CNGW_Message_Header_t));
    // lmcontrol_res.generic_header =  temp_header_ptr;

    // Reallocate the required memory size of the sendBuffer before copying the required command to it
    memset(sendBuffer, 0, sendBufferSize);
    sendBufferSize = sizeof(CCP_TX_FRAMES_t) * sizeof(uint8_t);
    uint8_t *resizedBuffer = realloc(sendBuffer, sendBufferSize);
    if (resizedBuffer == NULL)
    {
        printf("resizedBuffer memory allocation failed\n");
        return ESP_FAIL;
    }
    sendBuffer = resizedBuffer;

    // copy the data of the lmcontrol_res to sendBuffer
    memcpy(sendBuffer, &lmcontrol_res, sizeof(CCP_TX_FRAMES_t));
    return ESP_OK;
}

esp_err_t send_char_array()
{
    // create lmcontrol_res which contains the action command.
    char sendbuf[BUFFER] = "";
    sprintf(sendbuf, "data from ESP32 as a char array");
    // printf("size of sendbuf: %d\n", sizeof(sendbuf));
    memset(sendBuffer, 0, sendBufferSize);
    sendBufferSize = BUFFER * sizeof(uint8_t);
    uint8_t *resizedBuffer = realloc(sendBuffer, sendBufferSize);
    if (resizedBuffer == NULL)
    {
        printf("resizedBuffer memory allocation failed\n");
        return ESP_FAIL;
    }
    sendBuffer = resizedBuffer;
    // copy the data of the lmcontrol_res to sendBuffer
    memcpy(sendBuffer, sendbuf, sizeof(sendbuf));
    printf("sendBuffer data: %s\n", (char *)sendBuffer);
    return ESP_OK;
}
// Update the command to be sent to CN.
void CommandChange(int choice)
{
    esp_err_t ret = ESP_OK;
    if (xSemaphoreTake(sendBufferMutex, (TickType_t)0) != pdTRUE)
    {
        printf("CommandChange Failed to acquire the mutex\n");
        return;
    }
    switch (choice)
    {
    case 1:
        ret = dummy_Action_init();
        break;
    case 2:
        ret = send_char_array();
        break;
    default:
        printf("Command change, unidentified choice\n");
        break;
    }
    // printout if any error occured
    if (ret != ESP_OK)
    {
        printf("Error in CommandChange: %s\n", esp_err_to_name(ret));
    }
    // printout the contents of sendbuffer
    printf("sendbuffer: ");
    int i = 0;
    while (i < sizeof(sendBuffer))
    {
        printf("%02X", (int)sendBuffer[i]);
        i++;
    }
    printf("\n");
    printf("==================\n");
    xSemaphoreGive(sendBufferMutex);
}

void SPI_RxTx()
{
    // define slave transaction and set memory to zero
    CommandChange(1);
    spi_slave_transaction_t t;
    memset(&t, 0, sizeof(t));
    // create a recvbuf array and set memory to zero
    uint8_t recvbuf[BUFFER];
    memset(recvbuf, 0, sizeof(recvbuf));

    t.length = 128 * 8;
    // t.tx_buffer = NULL; //(uncomment if no data sending is needed)
    t.tx_buffer = sendBuffer;
    t.rx_buffer = recvbuf;
    esp_err_t ret;
    uint8_t counter = 0;
    bool handshake_done = false;
    while (1)
    {
        // after each spi transmit, reset the recvbuf memory
        memset(recvbuf, 0, sizeof(recvbuf));
        // set the slave transaction tx_buffer and rx_buffer to the created buffers above
        t.length = 128 * 8;
        if (handshake_done)
        {
            t.tx_buffer = NULL; //(uncomment if no data sending is needed)
        }
        else
        {
            t.tx_buffer = sendBuffer;
        }

        t.rx_buffer = recvbuf;
        // perform a SPI transaction. waits indefinetly until theres a returned item.
        ret = spi_slave_transmit(HSPI_HOST, &t, portMAX_DELAY);
        if (xSemaphoreTake(sendBufferMutex, (TickType_t)0) != pdTRUE)
        {
            printf("SPI_RxTx Failed to acquire the mutex\n");
            ret = ESP_FAIL;
        }
        if (t.tx_buffer != NULL)
        {
            gpio_set_level(GW_ON, 1);
            gpio_set_level(BDATA_READY, 0);

            vTaskDelay(pdMS_TO_TICKS(1));

            gpio_set_level(GW_ON, 0);
            gpio_set_level(BDATA_READY, 0);
        }
        else{
            ESP_LOGW(TAG, "tx_buffer is null");
            gpio_set_level(GW_ON, 0);
            gpio_set_level(BDATA_READY, 0);
        }

        // checks if there was a proper SPI transaction and that the data is not the echo of sent data
        if (ret == ESP_OK && memcmp(sendBuffer, recvbuf, sizeof(&sendBuffer)))
        {
            ESP_LOGI(TAG, "(int)recvbuf[1]: %d", (int)recvbuf[1]);
            if ((int)recvbuf[1] == 76)
            { // just for now, to filter out the cn1 handshake request
                analyze_CNGW_Handshake_CN1_t_data(recvbuf, BUFFER, true);
                handshake_done = false;
            }
            else if ((int)recvbuf[1] == 34)
            { // just for now, to filter out the cn1 handshake request
                analyze_CNGW_Handshake_CN2_t_data(recvbuf, BUFFER, true);
                handshake_done = false;
            }

            else if ((int)recvbuf[1] == 49)
            { // just for now, to filter out the cn1 handshake request
                // analyze_CNGW_Handshake_CN2_t_data(recvbuf, BUFFER, true);
                ESP_LOGW(TAG, "HANDSHAKE DONE!");
                handshake_done = true;
            }
            else{
               // handshake_done = false;

            }

            //
            counter++;
            /*
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
            */

            /*
             printf("Received:");
                 int i = 0;
                 while (i < sizeof(recvbuf))
                 {
                     printf("%02X ", (int)recvbuf[i]);
                     i++;
                 }
                 printf("\n");
            */
        }
        // either the spi transaction did not happen well, or the received data is the echo of sent data
        else
        {
            if (ret != ESP_OK)
            {
                printf("Error in SPI transaction. error code: %s\n", esp_err_to_name(ret));
            }
        }
        xSemaphoreGive(sendBufferMutex);
        // for debugging, the following logic switches between pre defined commands to be sent to CN
        // The switching is depending on the amount of messages received by ESP.
        // later, this command change should be done according to the type of commands received by ESP.
        if (counter == 10)
        {
            // CommandChange(2);
            // ESP_LOGE(TAG, "going to infinite loop");
            //  while(1){
            //      vTaskDelay(pdMS_TO_TICKS(5000));
            //  }
        }
        if (counter == 20)
        {
            //  CommandChange(1);
            counter = 0;
        }
    }
    ESP_LOGE(TAG, "SPI_RxTx Exit from while loop");
}

void app_main()
{ // Initialize the GPIO configuration
    gpio_config_t io_conf;
    io_conf.pin_bit_mask = (1ULL << BDATA_READY | 1ULL << GW_ON);
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    gpio_config(&io_conf);
    gpio_set_level(BDATA_READY, 0);
    gpio_set_level(GW_ON, 0);
    esp_err_t ret;
    ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
    {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);
    sendBufferMutex = xSemaphoreCreateMutex();
    if (sendBufferMutex == NULL)
    {
        ESP_LOGE(TAG, "Failed to create mutex");
        ESP_LOGI(TAG, "ESP restarting...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        esp_restart();
    }
    ret = init_SPI();
    if (ret == ESP_OK)
    {
        sendBuffer = malloc(sizeof(uint8_t));
        if (sendBuffer == NULL)
        {
            ESP_LOGE(TAG, "sendBuffer memory allocation failed");
        }
        xTaskCreatePinnedToCore(SPI_RxTx, "SPI_RxTx", 8 * 1024, NULL, 5, NULL, 0);
    }
    else
    {
        ESP_LOGE(TAG, "SPI failed to initialize. Error: %s", esp_err_to_name(ret));
        ESP_LOGI(TAG, "ESP restarting...");
        vTaskDelay(2000 / portTICK_PERIOD_MS);
        esp_restart();
    }
}

void analyze_CNGW_Handshake_CN2_t_data(const uint8_t *recvbuf, int size, bool print_data)
{
    CNGW_Handshake_CN2_Frame_t frame;
    memcpy(&frame, recvbuf, sizeof(frame));

    if (print_data)
    {
        ESP_LOGI(TAG, "frame decoded as CNGW_Handshake_CN2_Frame_t");
        printf("frame.header.command_type: %d\n", frame.header.command_type);
        printf("frame.header.data_size: %u\n", frame.header.data_size);
        printf("frame.header.crc: %u\n", frame.header.crc);

        printf("frame.message.command: %u\n", frame.message.command);
        printf("frame.message.status: %u\n", frame.message.status);
        printf("frame.message.hmac: ");
        for (int i = 0; i < CNGW_HMAC_LENGTH; i++)
        {
            printf("%u ", frame.message.hmac[i]);
        }
        printf("\n");

        ESP_LOGI(TAG, "--END--");
    }
    prepare_CN_handshake_02_feedback(&frame);
}

void analyze_CNGW_Handshake_CN1_t_data(const uint8_t *recvbuf, int size, bool print_data)
{
    CNGW_Handshake_CN1_Frame_t frame;
    memcpy(&frame, recvbuf, sizeof(frame));

    if (print_data)
    {
        ESP_LOGI(TAG, "frame decoded as CNGW_Handshake_CN1_Frame_t");
        printf("frame.header.command_type: %d\n", frame.header.command_type);
        printf("frame.header.data_size: %u\n", frame.header.data_size);
        printf("frame.header.crc: %u\n", frame.header.crc);
        printf("frame.message.cabinet_number: %u\n", frame.message.cabinet_number);
        printf("frame.message.challenge: ");
        for (int i = 0; i < CNGW_CHALLENGE_RESPONSE_LENGTH; i++)
        {
            printf("%u ", frame.message.challenge[i]);
        }
        printf("\n");
        printf("frame.message.command: %d\n", frame.message.command);
        printf("frame.message.hmac: ");
        for (int i = 0; i < CNGW_HMAC_LENGTH; i++)
        {
            printf("%u ", frame.message.hmac[i]);
        }
        printf("\n");
        printf("frame.message.mainboard_serial: ");
        for (int i = 0; i < CNGW_SERIAL_NUMBER_LENGTH; i++)
        {
            printf("%u ", frame.message.mainboard_serial[i]);
        }
        printf("\n");
        ESP_LOGI(TAG, "--END--");
    }
    prepare_CN_handshake_01_feedback(&frame);
}

void prepare_CN_handshake_02_feedback(CNGW_Handshake_CN2_Frame_t *cn)
{
    ESP_LOGI(TAG, "prepare_CN_handshake_02_feedback");
    const CNGW_Handshake_CN2_t *const cn2 = &cn->message;
    CNGW_Handshake_Status status = CNGW_Handshake_STATUS_SUCCESS;
    CNGW_Handshake_GW2_Frame_t gw2 = {0};
    // copy the received challenge to the output challenge
    // memcpy(gw2.message.challenge_response, &cn2->challenge, sizeof(gw2.message.challenge_response));
    // create the header
    CCP_UTIL_Get_Message_Header(&gw2.header, CNGW_HEADER_TYPE_Handshake_Response, sizeof(gw2.message));
    // setup the message field
    gw2.message.command = CNGW_Handshake_CMD_GW2; // CNGW_Handshake_CMD_GW1;
    gw2.message.status = status;
    // create dummy data about device serial
    // uint8_t gatewaySerial[CNGW_SERIAL_NUMBER_LENGTH] = {'A', 'B', 'C', '1', '2', '3', '4', '5', '6'};
    // memcpy(gw2.message.gateway_serial, gatewaySerial, CNGW_SERIAL_NUMBER_LENGTH);
    // gw2.message.gateway_model = 0;
    // gw2.message.firmware_version = *GWV_Get_Firmware();
    // gw2.message.bootloader_version = *GWV_Get_Bootloader_Firmware();
    // copy the hmac of the cn mcu to the response hmac
    memcpy(gw2.message.hmac, &cn2->hmac, sizeof(gw2.message.hmac));
    // sending the frame
    size_t sizee = sizeof(gw2);
    ESP_LOGE(TAG, "size of gw: %zu", sizee);
    // const CCP_TX_FRAMES_t frame = { .gw2 = &gw2 };
    // sizee = sizeof(frame);
    // ESP_LOGE(TAG, "size of frame: %zu", sizee);
    memset(sendBuffer, 0, sendBufferSize);                        // clear out the whole of previous stored memory
    sendBufferSize = sizeof(gw2) * sizeof(uint8_t);               // get the required new size
    uint8_t *resizedBuffer = realloc(sendBuffer, sendBufferSize); // re-allocate required memory for the sendBuffer
    if (resizedBuffer == NULL)
    {
        printf("resizedBuffer memory allocation failed\n");
    }
    sendBuffer = resizedBuffer;
    // copy the data of the lmcontrol_res to sendBuffer
    memset(sendBuffer, 0, sendBufferSize);
    memcpy(sendBuffer, &gw2, sizeof(CNGW_Handshake_GW2_Frame_t));
    ESP_LOGI(TAG, "Sending buffer of size: %d\n", sendBufferSize);

    /*
    printf("sendBuffer data:\n");
    for (size_t i = 0; i < sendBufferSize; i++)
    {
        printf("%d\t%02X\n", i, sendBuffer[i]);
    }
    printf("\n");
    */
}

void prepare_CN_handshake_01_feedback(CNGW_Handshake_CN1_Frame_t *cn)
{
    ESP_LOGI(TAG, "prepare_CN_handshake_01_feedback");
    const CNGW_Handshake_CN1_t *const cn1 = &cn->message;
    CNGW_Handshake_Status status = CNGW_Handshake_STATUS_SUCCESS;
    CNGW_Handshake_GW1_Frame_t gw1 = {0};
    // copy the received challenge to the output challenge
    memcpy(gw1.message.challenge_response, &cn1->challenge, sizeof(gw1.message.challenge_response));
    // create the header
    CCP_UTIL_Get_Message_Header(&gw1.header, CNGW_HEADER_TYPE_Handshake_Response, sizeof(gw1.message));
    // setup the message field
    gw1.message.command = CNGW_Handshake_CMD_GW1; // CNGW_Handshake_CMD_GW1;
    gw1.message.status = status;
    // create dummy data about device serial
    uint8_t gatewaySerial[CNGW_SERIAL_NUMBER_LENGTH] = {'A', 'B', 'C', '1', '2', '3', '4', '5', '6'};
    memcpy(gw1.message.gateway_serial, gatewaySerial, CNGW_SERIAL_NUMBER_LENGTH);
    gw1.message.gateway_model = 0;
    gw1.message.firmware_version = *GWV_Get_Firmware();
    gw1.message.bootloader_version = *GWV_Get_Bootloader_Firmware();
    // copy the hmac of the cn mcu to the response hmac
    memcpy(gw1.message.hmac, &cn1->hmac, sizeof(gw1.message.hmac));
    // sending the frame
    size_t sizee = sizeof(gw1);
    ESP_LOGE(TAG, "size of gw: %zu", sizee);
    // const CCP_TX_FRAMES_t frame = { .gw1 = &gw1 };
    // sizee = sizeof(frame);
    // ESP_LOGE(TAG, "size of frame: %zu", sizee);
    memset(sendBuffer, 0, sendBufferSize);                        // clear out the whole of previous stored memory
    sendBufferSize = sizeof(gw1) * sizeof(uint8_t);               // get the required new size
    uint8_t *resizedBuffer = realloc(sendBuffer, sendBufferSize); // re-allocate required memory for the sendBuffer
    if (resizedBuffer == NULL)
    {
        printf("resizedBuffer memory allocation failed\n");
    }
    sendBuffer = resizedBuffer;
    // copy the data of the lmcontrol_res to sendBuffer
    memset(sendBuffer, 0, sendBufferSize);
    memcpy(sendBuffer, &gw1, sizeof(CNGW_Handshake_GW1_Frame_t));
    ESP_LOGI(TAG, "Sending buffer of size: %d\n", sendBufferSize);

    /*
    printf("sendBuffer data:\n");
    for (size_t i = 0; i < sendBufferSize; i++)
    {
        printf("%d\t%02X\n", i, sendBuffer[i]);
    }
    printf("\n");
    */
}

void CCP_UTIL_Get_Message_Header(CNGW_Message_Header_t *const header, CNGW_Header_Type command, const size_t data_size)
{
    // ASSERT_DEBUG_MODE(header);
    // ASSERT_DEBUG_MODE(data_size > 0);

    size_t crc_size = sizeof(*header) - sizeof(header->crc);

    header->command_type = command;
    CNGW_SET_HEADER_DATA_SIZE(header, data_size);
    header->crc = CCP_UTIL_Get_Crc8(0, (uint8_t *)header, crc_size);
}

static const CNGW_Firmware_Version_t current_firmware_version =
    {
        .major = MAJOR_VER,
        .minor = MINOR_VER,
        .ci = CI_BLD_NUM,
        .branch_id = GIT_BRCH_ID};

const CNGW_Firmware_Version_t *GWV_Get_Firmware(void)
{
    return &current_firmware_version;
}
const CNGW_Firmware_Version_t *GWV_Get_Bootloader_Firmware(void)
{
    return &current_firmware_version;
}
