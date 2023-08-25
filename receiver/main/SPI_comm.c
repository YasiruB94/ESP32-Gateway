#include "inc/SPI_comm.h"
static const char *TAG = "SPI_comm";
SemaphoreHandle_t dataMutex;
QueueHandle_t GW_response_queue;
QueueHandle_t CN_message_queue;
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
        .queue_size = 10,
        .flags = 0};

    // Enable pull-ups on SPI lines so we don't detect rogue pulses when no master is connected.
    gpio_set_pull_mode(GPIO_MOSI, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_SCLK, GPIO_PULLUP_ONLY);
    gpio_set_pull_mode(GPIO_CS, GPIO_PULLUP_ONLY);
    // initialize the semaphore
    dataMutex = xSemaphoreCreateMutex();
    // initialize the GW_response queue
    GW_response_queue = xQueueCreate(QUEUE_LENGTH, BUFFER * sizeof(uint8_t));
    // initialize the CN_message queue
    CN_message_queue = xQueueCreate(QUEUE_LENGTH, BUFFER * sizeof(uint8_t));
    // Initialize SPI slave interface
    return spi_slave_initialize(HSPI_HOST, &buscfg, &slvcfg, 1);
}

void GW_Send_Data(void *pvParameters)
{
    ESP_LOGI(TAG, "GW_Send_Data");
    uint8_t data_to_send[] = "Hello, Master!";
    spi_slave_transaction_t trans;
    memset(&trans, 0, sizeof(trans));
    while (1)
    {
        if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE)
        {
            trans.tx_buffer = data_to_send;
            trans.rx_buffer = NULL;
            trans.length = 128 * 8;

            // Transmit data to master
            Signal_GW_Of_Sending_Data();
            esp_err_t ret = spi_slave_transmit(HSPI_HOST, &trans, 300);
            if (ret != ESP_OK)
            {
                ESP_LOGE(TAG, "SPI transmit error: %d", ret);
            }
            xSemaphoreGive(dataMutex);
        }
    }
}

void GW_Receive_Data(void *pvParameters)
{
    ESP_LOGI(TAG, "GW_Receive_Data");
    uint8_t recvbuf[BUFFER];
    spi_slave_transaction_t trans;
    memset(&trans, 0, sizeof(trans));
    while (1)
    {
        if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE)
        {
            memset(recvbuf, 0, sizeof(recvbuf));
            trans.length = 128 * 8;
            trans.tx_buffer = NULL;
            trans.rx_buffer = recvbuf;
            // Receive data from master
            esp_err_t ret = spi_slave_transmit(HSPI_HOST, &trans, 300);
            // esp_err_t ret = spi_slave_queue_trans(HSPI_HOST, &trans, 300);
            ESP_LOGI(TAG, "GW_Received_Data. Status: %d", ret);
            if (ret == ESP_OK)
            {
                printf("Received:");
                int i = 0;
                while (i < sizeof(recvbuf))
                {
                    printf("%02X ", (int)recvbuf[i]);
                    i++;
                }
                printf("\n");
            }
            xSemaphoreGive(dataMutex);
        }
    }
}

void GW_Trancieve_Data(void *pvParameters)
{
    bool print_recv_data = false;
    ESP_LOGI(TAG, "GW_Trancieve_Data");
    uint8_t data_to_send[BUFFER];
    uint8_t *recvbuf = (uint8_t *)heap_caps_malloc(BUFFER, MALLOC_CAP_DMA);
    spi_slave_transaction_t trans;
    memset(&trans, 0, sizeof(trans));
    while (1)
    {
        if (xSemaphoreTake(dataMutex, portMAX_DELAY) == pdTRUE)
        {
            memset(recvbuf, 0, BUFFER);
            memset(data_to_send, 0, BUFFER);
            if (uxQueueMessagesWaiting(GW_response_queue) > 0)
            {
                // there are messages waiting to be sent to the CN
               // if (xQueuePeek(GW_response_queue, data_to_send, 0) == pdTRUE)
                if (xQueueReceive(GW_response_queue, data_to_send, 0) == pdTRUE)
                {
                    // got the data to the data_to_send buffer
                    trans.tx_buffer = data_to_send;
                        Signal_GW_Of_Sending_Data();
                    
                }
                else
                { // failed to read data from the queue
                    trans.tx_buffer = NULL;
                }
            }
            else
            {
                // there are no messages to be sent
                trans.tx_buffer = NULL;
            }

            trans.rx_buffer = recvbuf;
            trans.length = 200*8;//129*8
            // Receive data from master

            esp_err_t ret = spi_slave_transmit(HSPI_HOST, &trans, 1);
            Signal_GW_Of_Receiving_Data();
            if (ret == ESP_OK)
            {
                if (memcmp(data_to_send, recvbuf, sizeof(&data_to_send)) == 0)
                {
                    // the data is the same as the sent data. now can remove this message from the queue
                   // xQueueReceive(GW_response_queue, data_to_send, 0);
                }
                else
                {
                    // data from the CN
                    if (is_all_zeros(recvbuf, BUFFER) != ESP_OK)
                    {
                        if (xQueueSend(CN_message_queue, recvbuf, 50) != pdTRUE)
                        {
                            ESP_LOGE(TAG, "CN_message_queue is full");
                        }
                        if (print_recv_data)
                        {
                            printf("Received:");
                            int i = 0;
                            while (i < BUFFER)
                            {
                                printf("%02X ", (int)recvbuf[i]);
                                i++;
                            }
                            printf("\n");
                        }
                    }
                }
            }
            xSemaphoreGive(dataMutex);
        }
    }
}

esp_err_t is_all_zeros(const uint8_t *array, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        if ((int)array[i] != 0)
        {
            return ESP_FAIL; // If any element is non-zero, return false
        }
    }

    return ESP_OK; // All elements are zero
}

esp_err_t consume_GW_message(uint8_t *message)
{    ESP_LOGI(TAG, "Adding message to queue");
    if (xQueueSend(GW_response_queue, message, 100) == pdTRUE)
    {
        return ESP_OK;
    }
    else
    {
        return ESP_FAIL;
    }
}

