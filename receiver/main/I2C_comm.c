#include "inc/I2C_comm.h"
 #include "include/atecc508a.h"
static const char *TAG = "I2C_comm";

esp_err_t init_I2C()
{
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO, // select SDA GPIO specific to your project
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = I2C_MASTER_SCL_IO, // select SCL GPIO specific to your project
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ, // select frequency specific to your project
        //.master.clk_flags = 0,                          // optional; you can use I2C_SCLK_SRC_FLAG_* flags to choose i2c source clock here
    };

    i2c_param_config(I2C_NUM_0, &conf);
    i2c_driver_install(I2C_NUM_0, I2C_MODE_MASTER, 0, 0, 0);

    return ESP_OK;
}

void I2C_read(void *pvParameters)
{
    ESP_LOGI(TAG, "I2C read init");
    while (1)
    {
        ESP_LOGI(TAG, "I2C read");
        uint8_t random[35] = {0};
        esp_err_t ret =  atecc508a_random(random, 0);
        printf("Values in random array state: %s\n", esp_err_to_name(ret));
        for (int i = 0; i < sizeof(random) / sizeof(random[0]); i++)
        {
            printf("%u ", random[i]); // Print as unsigned decimal
            // Alternatively, you can print as hexadecimal: printf("0x%02X ", random[i]);
        }
        printf("\n");
        /*
      uint8_t data_received = 0;
      i2c_cmd_handle_t cmd = i2c_cmd_link_create();
      i2c_master_start(cmd);
      i2c_master_write_byte(cmd, I2C_7BIT_ADDRESS << 1 | I2C_MASTER_READ, true);
      i2c_master_read_byte(cmd, &data_received, I2C_MASTER_NACK);
      i2c_master_stop(cmd);
      esp_err_t ret = i2c_master_cmd_begin(I2C_NUM_0, cmd, 1000 / portTICK_RATE_MS);
      if(ret != ESP_OK){
          ESP_LOGE(TAG,"error: %s", esp_err_to_name(ret));
      }
      i2c_cmd_link_delete(cmd);
      printf("Received data: %u\n", data_received);       // Print as decimal
      printf("Received data: 0x%02X\n", data_received);   // Print as hexadecimal
      */
        // uint8_t response[35] = {};
        // ESP_CHECK_RET(atecc508a_send_command(ATECC508A_CMD_RANDOM, 0x00, 0x0000, NULL, 0));

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
