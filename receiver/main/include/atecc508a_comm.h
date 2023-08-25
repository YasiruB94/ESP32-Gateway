#pragma once
#define ATCA_HMAC                       ((uint8_t)0x11)         //!< HMAC command op-code
#define HMAC_MODE_SOURCE_FLAG_MATCH ((uint8_t)0x04)     //!< HMAC mode bit 2: match TempKey.SourceFlag
#define ATCA_CMD_SIZE_MIN       ((uint8_t)7)
#define HMAC_COUNT                  ATCA_CMD_SIZE_MIN   //!< HMAC command packet size
#define ATCA_RSP_SIZE_32            ((uint8_t)35)                       //!< size of response packet containing 32 bytes data
#define HMAC_RSP_SIZE               ATCA_RSP_SIZE_32    //!< response size of HMAC command

typedef struct ATCAPACKE_t
{

    uint8_t     word_address;  /**HAL layer as needed (I/O tokens, Word address values)*/

    /**--- start of packet i/o frame----*/
    uint8_t     txsize;
    uint8_t     opcode;
    uint8_t     param1;
    uint16_t    param2;

    /** includes 2-byte CRC.  data size is determined by largest possible data section of any
     command + crc (see: x08 verify data1 + data2 + data3 + data4)
     this is an explicit design trade-off (space) resulting in simplicity in use
     and implementation*/
    uint8_t     data[130];
    /**--- end of packet i/o frame ---*/

    /**--- Used for receiving ---*/
    /*uint8_t execTime;*/
    uint16_t rxsize;        /**expected response size, response is held in data member*/

} __attribute__((packed)) ATCAPACKE_t;

typedef enum
{
    ATECC508A_CMD_READ = 0x02,
    ATECC508A_CMD_WRITE = 0x12,
    ATECC508A_CMD_LOCK = 0x17,
    ATECC508A_CMD_RANDOM = 0x1B,
} atecc508a_command_t;

/**
 * @brief
 *
 * @param command
 * @param param1
 * @param param2
 * @param data
 * @param length
 * @return esp_err_t
 */
esp_err_t atecc508a_send_command(atecc508a_command_t command, uint8_t param1, uint16_t param2, uint8_t *data, size_t length);

esp_err_t atecc508a_send_cmnd(atecc508a_command_t command, uint8_t param1, uint16_t param2, uint8_t *data, size_t length);
esp_err_t atecc508a_send(ATCAPACKE_t *packet);

/**
 * @brief
 *
 * @param buffer
 * @param length
 * @return esp_err_t
 */
esp_err_t atecc508a_receive(uint8_t *buffer, size_t length);

/**
 * @brief
 *
 * @param buffer
 * @param length
 * @return esp_err_t
 */
esp_err_t atecc508a_receive(uint8_t *buffer, size_t length);

/**
 * @brief Reads data from the IC at a specific zone and address.
 *
 * @param zone
 * @param address
 * @param buffer
 * @param length
 * @return esp_err_t
 */
esp_err_t atecc508a_read(uint8_t zone, uint16_t address, uint8_t *buffer, uint8_t length);

/**
 * @brief
 *
 * @param zone
 * @param address
 * @param buffer
 * @param length
 * @return esp_err_t
 */
esp_err_t atecc508a_write(uint8_t zone, uint16_t address, uint8_t *buffer, uint8_t length);