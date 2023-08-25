#include <string.h>

#include <esp_err.h>
#include <esp_log.h>

#include <driver/i2c.h>
#include <inc/temp.h>

#include "include/atecc508a.h"
#include "include/atecc508a_comm.h"
#include "include/atecc508a_crc.h"
#include "include/atecc508a_util.h"
#include "mbedtls/sha256.h"
#include "sdkconfig.h"

#define LOG_TAG "atecc508a"
static const uint8_t CODE_AUTHORIZATION_KEY_SLOT = 2;
esp_err_t atecc508a_init()
{
    i2c_config_t config = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = CONFIG_ATECC508A_I2C_MASTER_SDA,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_io_num = CONFIG_ATECC508A_I2C_MASTER_SCL,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = CONFIG_ATECC508A_I2C_MASTER_FREQUENCY};

    i2c_param_config(ATECC508A_PORT, &config);

    i2c_driver_install(ATECC508A_PORT, config.mode, 0, 0, 0);

    ESP_LOGI(LOG_TAG, "ATECC508A I2C Init done");

    return ESP_OK;
}

esp_err_t atecc508a_random(uint8_t *random, uint8_t mode)
{
    uint8_t response[35] = {};
    uint8_t data[2] = {1, 2};
    // send command
    // atecc508a_send_command(ATECC508A_CMD_RANDOM, 0x00, 0x0000, NULL, 0); //original
    atecc508a_send_command(ATCA_HMAC, HMAC_MODE_SOURCE_FLAG_MATCH, 3, data, sizeof(data)); // ATECC508A_CMD_READ

    atecc508a_delay(23);

    atecc508a_receive(response, sizeof(response));

    atecc508a_idle();

    atecc508a_check_crc(response, sizeof(response));

    memcpy(random, response + 1, 32);

    return ESP_OK;
}
static void Get_Address(const uint8_t zone, const uint8_t slot, const uint8_t block, uint8_t offset, uint16_t *address)
{

    uint8_t memzone = zone & 0x03;

    // Initialize the addr to 00
    *address = 0;
    // Mask the offset
    offset = offset & (uint8_t)0x07;
    if ((memzone == ATCA_ZONE_CONFIG) || (memzone == ATCA_ZONE_OTP))
    {
        *address = block << 3;
        *address |= offset;
    }
    else
    { // ATCA_ZONE_DATA
        *address = slot << 3;
        *address |= offset;
        *address |= block << 8;
    }
}
void ATMELPLA_Generate_Salt(uint8_t *salt)
{
    uint32_t rnd;

    for (uint32_t i = 0; i < 5; i++)
    {
        const uint32_t pos = i * 4;
        rnd = rand();

        salt[pos] = rnd & 0xFF;
        salt[pos + 1] = ((rnd >> 8) & 0xFF);
        salt[pos + 2] = ((rnd >> 16) & 0xFF);
        salt[pos + 3] = ((rnd >> 24) & 0xFF);
    }
}
const uint8_t *OS_Get_512_Zeros(void)
{
    static const uint8_t flash_zero[512] = {0};
    return flash_zero;
}
static inline void SHA256_Zero(mbedtls_sha256_context *ctx, const uint16_t count)
{
    const uint8_t *zeros = OS_Get_512_Zeros();
    mbedtls_sha256_update_ret(ctx, zeros, count);
}

GW_STATUS ATMEL_Nonce_Random(const uint8_t *const salt, uint8_t *const nonce)
{
    uint8_t response[35] = {0};
    atecc508a_send_command(0x16, 0x0000, 0, salt, sizeof(salt));
    atecc508a_delay(23);
    atecc508a_receive(response, sizeof(response));
    atecc508a_idle();
    atecc508a_check_crc(response, sizeof(response));
    printf("ATMEL_Nonce_Random: ");
    for (int i = 0; i < sizeof(response); i++)
    {
        printf("0x%x ", response[i]);
    }
    printf("\n");

    mbedtls_sha256_context ctx;

    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts_ret(&ctx, 0);
    mbedtls_sha256_update_ret(&ctx, response, 32);
    mbedtls_sha256_update_ret(&ctx, salt, 20);
    // mbedtls_sha256_update_ret(&ctx, &packet.opcode, 1U);
    // mbedtls_sha256_update_ret(&ctx, &packet.param1, 1U);
    SHA256_Zero(&ctx, 1U);
    mbedtls_sha256_finish_ret(&ctx, nonce);

    return GW_STATUS_GENERIC_SUCCESS;
}
void ATMELUTIL_Get_Code_Authorization(uint8_t *const key)
{
    static const uint8_t slot_data[] =
        {
            /*Forced in flash*/
            0x69, 0xf6, 0x0f, 0x46, 0xd0, 0xf4, 0x02,
            0xb7, 0x8e, 0x25, 0x2c, 0x20, 0x94, 0x3c,
            0xff, 0xac, 0x1f, 0x03, 0x73, 0x7b, 0x99,
            0x43, 0x2f, 0xa5, 0x84, 0x68, 0xbd, 0x68,
            0x64, 0xa4, 0x1f, 0x1f};

    // ASSERT_DEBUG_MODE(sizeof(slot_data) >= ATMEL_KEY_SIZE);

    uint16_t i = 32;

    while (i--)
    {
        key[i] = slot_data[i] ^ 0xA3;
    }
}

GW_STATUS ATMEL_Nonce_Passthrough(const uint8_t *const tempkey)
{    uint8_t response[4] = {0};
    // ASSERT_DEBUG_RETURN_RELEASE(tempkey, GW_STATUS_GENERIC_BAD_PARAM);

    atecc508a_send_command(ATCA_NONCE, NONCE_MODE_PASSTHROUGH, 0, tempkey, sizeof(tempkey));
    atecc508a_delay(23);
    atecc508a_receive(response, sizeof(response));
    atecc508a_idle();
    atecc508a_check_crc(response, sizeof(response));
    printf("ATMEL_Nonce_Passthrough response: ");
    for (int i = 0; i < sizeof(response); i++)
    {
        printf("0x%x ", response[i]);
    }
    printf("\n");

    return true;
}

esp_err_t ATMEL_HMAC_1(const uint8_t *const message, const uint32_t length, const uint8_t key_slot, uint8_t *hmac)
{

    ESP_LOGI(LOG_TAG, "ATMEL_HMAC_1");
    GW_STATUS status = GW_STATUS_GENERIC_TIMEOUT;

    if (NULL == hmac)
    {
        hmac = (uint8_t *)(message + length);
    }
    uint8_t digest[ATMEL_HASH_SIZE] = {0};
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts_ret(&ctx, 0);
    mbedtls_sha256_update_ret(&ctx, message, length);
    mbedtls_sha256_finish_ret(&ctx, digest);

    ATCAPACKET_t packet = {0};
    /*
            packet.word_address = 0x03;
            packet.opcode       = ATCA_HMAC;
            packet.param1       = HMAC_MODE_SOURCE_FLAG_MATCH;
            packet.param2       = key_slot;
            packet.txsize       = HMAC_COUNT;
            packet.rxsize       = HMAC_RSP_SIZE;
    */
    // uint8_t total_transmission_length = 1 + 1 + 1 + sizeof(0x00) + sizeof(0x0000) + 0 + sizeof(uint16_t);
    // ESP_LOGI(LOG_TAG,"total_transmission_length: %d", total_transmission_length);
    ///*
    packet.word_address = 0x03;
    packet.opcode = ATECC508A_CMD_RANDOM;
    packet.param1 = 0x00;
    packet.param2 = 0x00;
    // packet.data         = {0};
    packet.txsize = HMAC_COUNT;
    packet.rxsize = HMAC_RSP_SIZE;
    //*/

    // ATMELUTIL_Calculate_CRC(&packet);
    // atecc508a_send(&packet); //ATECC508A_CMD_READ
    // atecc508a_send_command(ATECC508A_CMD_RANDOM, 0x00, 0x0000, NULL, 0); //original
    /*
    //READ command
    uint8_t response[4] = {0};//4
    atecc508a_send_command(0x02, 0, 0x0048, NULL, 0);
    */
    /*
    //HMAC command
    uint8_t response[4] = {0};//if error:4
    atecc508a_send_command(0x11, 0x00, 3, NULL, 0);
    */
    /*
    //RANDOM command
    uint8_t response[35] = {0};//32+3
    atecc508a_send_command(0x1B, 0x00, 0x00, NULL, 0);
    */
    /*
    //INFO command
    uint8_t response[7] = {0};//4+3
    atecc508a_send_command(0x30, 0x00, 0x00, NULL, 0);
    */

    /*
    //NONCE command
    uint8_t response[35] = {0};//32+3
    uint8_t salt[20]   = { 0 };
    uint8_t nonce[32] = { 0 };
    atecc508a_send_command(0x16, 0x0000, 0, salt, sizeof(salt));
    */
    /*
 //NONCE with digest command
 uint8_t response[4] = {0};
 atecc508a_send_command(0x16, 0x03, 0, digest, sizeof(digest));
 */

    /*
    //NONCE with random command
    uint8_t response[35] = {0};
        uint8_t salt[20]   = { 0 };
    uint8_t nonce[32] = { 0 };
    atecc508a_send_command(0x16, 0x00, 0, salt, sizeof(salt));
    */

    // authorize_MAC------------------------------------------------------------------
    do
    {
        uint8_t serial[ATMEL_SERIAL_LENGTH] = {0};
        // READ1 command
        uint8_t read_1_response[4] = {0};
        uint16_t address_1;
        Get_Address(ATCA_ZONE_CONFIG, 0, 0, 0, &address_1);
        atecc508a_send_command(ATCA_READ, ATCA_ZONE_CONFIG, address_1, NULL, 0);
        atecc508a_delay(23);
        atecc508a_receive(read_1_response, sizeof(read_1_response));
        atecc508a_idle();
        atecc508a_check_crc(read_1_response, sizeof(read_1_response));
        memcpy(serial, read_1_response, sizeof(read_1_response));
        printf("read_1_response: ");
        for (int i = 0; i < sizeof(read_1_response); i++)
        {
            printf("0x%x ", read_1_response[i]);
        }
        printf("\n");

        // READ2 command
        uint8_t read_2_response[4] = {0};
        uint16_t address_2;
        Get_Address(ATCA_ZONE_CONFIG, 0, 0, 2, &address_2);
        atecc508a_send_command(ATCA_READ, ATCA_ZONE_CONFIG, address_2, NULL, 0);
        atecc508a_delay(23);
        atecc508a_receive(read_2_response, sizeof(read_2_response));
        atecc508a_idle();
        atecc508a_check_crc(read_2_response, sizeof(read_2_response));
        memcpy(serial + 3, read_2_response, sizeof(read_2_response));
        printf("read_2_response: ");
        for (int i = 0; i < sizeof(read_2_response); i++)
        {
            printf("0x%x ", read_2_response[i]);
        }
        printf("\n");

        // READ3 command
        uint8_t read_3_response[4] = {0};
        uint16_t address_3;
        Get_Address(ATCA_ZONE_CONFIG, 0, 0, 3, &address_3);
        atecc508a_send_command(ATCA_READ, ATCA_ZONE_CONFIG, address_3, NULL, 0);
        atecc508a_delay(23);
        atecc508a_receive(read_3_response, sizeof(read_3_response));
        atecc508a_idle();
        atecc508a_check_crc(read_3_response, sizeof(read_3_response));
        memcpy(serial + 7, read_3_response, sizeof(read_3_response));
        printf("read_3_response: ");
        for (int i = 0; i < sizeof(read_3_response); i++)
        {
            printf("0x%x ", read_3_response[i]);
        }
        printf("\n");

        printf("serial: ");
        for (int i = 0; i < sizeof(serial); i++)
        {
            printf("0x%x ", serial[i]);
        }
        printf("\n");

        uint8_t salt[20] = {0};
        uint8_t nonce[32] = {0};
        GW_STATUS status = ATMEL_Nonce_Random(salt, nonce);
        memset(salt, 0, sizeof(salt));

        /*Calculate authorization MAC*/
        uint8_t key[32];
        mbedtls_sha256_context ctx;

        ATMELUTIL_Get_Code_Authorization(key);

        mbedtls_sha256_init(&ctx);
        mbedtls_sha256_starts_ret(&ctx, 0);

        mbedtls_sha256_update_ret(&ctx, key, 32);
        mbedtls_sha256_update_ret(&ctx, nonce, 32);
        mbedtls_sha256_update_ret(&ctx, salt, 4U);
        SHA256_Zero(&ctx, 8);
        mbedtls_sha256_update_ret(&ctx, &salt[4], 3U);
        mbedtls_sha256_update_ret(&ctx, &serial[8], 1U);
        mbedtls_sha256_update_ret(&ctx, &salt[7], 4U);
        mbedtls_sha256_update_ret(&ctx, serial, 2U);
        mbedtls_sha256_update_ret(&ctx, &salt[11], 2U);

        uint8_t send_data_4[130] = {0};
        uint8_t response_4[4] = {0};
        mbedtls_sha256_finish_ret(&ctx, &send_data_4[32]);
        memcpy(&send_data_4[32 + 32], salt, 13);
        atecc508a_send_command(ATCA_CHECKMAC, CHECKMAC_MODE_BLOCK2_TEMPKEY, CODE_AUTHORIZATION_KEY_SLOT, send_data_4, sizeof(send_data_4));
        atecc508a_delay(23);
        atecc508a_receive(response_4, sizeof(response_4));
        atecc508a_idle();
        atecc508a_check_crc(response_4, sizeof(response_4));

        printf("response_4: ");
        for (int i = 0; i < sizeof(response_4); i++)
        {
            printf("0x%x ", response_4[i]);
        }
        printf("\n");

    } while (0);

    //------------------------------------------------------------------------------------
    // ATMEL Nonce passthrough
    ATMEL_Nonce_Passthrough(digest);

    //-------------------------------------------------------------------------------------
    //  /*
    // HMAC command
    uint8_t response[35] = {0};
    atecc508a_send_command(ATCA_HMAC, HMAC_MODE_SOURCE_FLAG_MATCH, 3, NULL, 0);
    //*/

    atecc508a_delay(23);
    atecc508a_receive(response, sizeof(response));
    atecc508a_idle();
    atecc508a_check_crc(response, sizeof(response));

    memcpy(hmac, response, sizeof(response));
    return ESP_OK;
}

esp_err_t atecc_508a_send_and_receive(uint8_t *result)
{
    uint8_t response[35] = {};

    // atecc508a_send_cmnd(ATECC508A_CMD_RANDOM, 0x00, 0x0000, NULL, 0); //ATECC508A_CMD_READ
    atecc508a_send_cmnd(ATCA_HMAC, HMAC_MODE_SOURCE_FLAG_MATCH, 3, NULL, 0); // ATECC508A_CMD_READ

    atecc508a_delay(23);

    atecc508a_receive(response, sizeof(response));

    atecc508a_idle();

    atecc508a_check_crc(response, sizeof(response));

    memcpy(result, response + 1, 32);

    return ESP_OK;
}
