
#define GW_SPI_RX_MAX_BUFFER_SIZE           (144u)                                      /*Maximum RX DMA buffer for Gateway MCU. I.e Sizeof(CNGW_Log_Message_Frame_t)*/
#define RX_BUFFER_SIZE                  (GW_SPI_RX_MAX_BUFFER_SIZE * 4u)

#define CNGW_SERIAL_NUMBER_LENGTH           (9U)
#define CNGW_CHALLENGE_RESPONSE_LENGTH      (32U)
#define CNGW_HMAC_LENGTH                    (32U)

typedef struct Buffer_t
{
    uint8_t volatile *buffer;
    uint16_t volatile size;
    uint16_t maxSize;
} Buffer_t;

/**
 * @brief Used by the RX_BUFFER to stored
 * the location of the data that was received
 * which need parsing
 */
struct CCP_PACKET
{
    const uint8_t *start;
    const uint8_t *end;
};
typedef struct CCP_PACKET CCP_PACKET_t;

/**
 * @brief RX DMA buffer
 */
struct CCP_RX
{
    volatile uint8_t buffer[RX_BUFFER_SIZE];

    /* Track where the dma wrote the first byte
     * of the packet*/
    volatile uint8_t *volatile dma_start;

    /*When A possible lost in RX can occur this is set.
     * This occurs if the consumer cannot keep up with the producer*/
    volatile uint8_t is_miss;

    /*Holds the pointer location of the packets to be parsed*/
    QueueHandle_t packet_queue;

};
typedef struct CCP_RX CCP_RX_t;



typedef enum CNGW_Handshake_Command
{
	CNGW_Handshake_CMD_CN1  = 0x01,
	CNGW_Handshake_CMD_GW1  = 0x04,
	CNGW_Handshake_CMD_CN2  = 0x02,
	CNGW_Handshake_CMD_GW2  = 0x03,
} __attribute__((packed)) CNGW_Handshake_Command;


typedef enum CNGW_Handshake_Status
{
	CNGW_Handshake_STATUS_SUCCESS   = 0x01,
	CNGW_Handshake_STATUS_FAILED    = 0x02
} __attribute__((packed)) CNGW_Handshake_Status;

/**
 * @brief Structure for the initial handshake CN1 message between Control MCU and Gateway MCU
 *
 * The #CNGW_Handshake_CMD_CN1 is the beginning message to the handshake process.  It includes
 * the serial number of the mainboard as well as a challenge to be used in a
 * challenge / response process
 *
 * Message is 94 bytes in length (not including header)
 *
 * @note This is send under the header type #CNGW_HEADER_TYPE_Handshake_Command
 */
typedef struct CNGW_Handshake_CN1_t
{
	CNGW_Handshake_Command command;
	uint8_t     mainboard_serial[CNGW_SERIAL_NUMBER_LENGTH];    /** @brief the Serial number of the CENSE mainboard */
	uint16_t    cabinet_number;                                 /** @brief The cabinet number as indicated by the CN MCU*/
	uint8_t     challenge[CNGW_CHALLENGE_RESPONSE_LENGTH];      /** @brief the challenge/response challenge from the mainboard */
	uint8_t     hmac[CNGW_HMAC_LENGTH];                         /** @brief HMAC of this entire structure (excluding the HMAC itself */

}  __attribute__((packed)) CNGW_Handshake_CN1_t;

/**
 * @brief A generic ACK message sent from the MCU as the last
 *        message from CN or GW
 */
typedef struct CNGW_Handshake_Ack_t
{
	CNGW_Handshake_Command  command;    /** @brief Must CNGW_Handshake_CMD_xx2. Depending on if originating from CN or GW*/
	CNGW_Handshake_Status   status;	    /** @brief The handshake status depending on fail or pass*/
	uint8_t hmac[CNGW_HMAC_LENGTH];     /** @brief HMAC of this entire structure (excluding the HMAC itself */
} __attribute__((packed)) CNGW_Handshake_Ack_t;

/**
 * @brief The ACK message for CN2 sent as
 *        CNGW_HEADER_TYPE_Handshake_Command
 */
typedef CNGW_Handshake_Ack_t CNGW_Handshake_CN2_t;



/**
 * @brief Memory space for the largest possible CN message
 */
typedef union CCP_HANDSHAKE_CN_MESSAGES_t
{
    /**@brief The command field for the messages below.
     * This command field is always the first byte in each
     * message*/
    CNGW_Handshake_Command  command;

    CNGW_Handshake_CN1_t    cn1_message;
    CNGW_Handshake_CN2_t    cn2_message;
} __attribute__((packed)) CCP_HANDSHAKE_CN_MESSAGES_t;

/**@brief Header type define a class of messages.
 *        These values are used for bit position shifting.
 *        The largest value must not exceed 32*/
typedef enum CNGW_Header_Type
{
	CNGW_HEADER_TYPE_Action_Commmand                = 0x01,
	CNGW_HEADER_TYPE_Query_Command                  = 0x02,
	CNGW_HEADER_TYPE_Configuration_Command          = 0x03,
	CNGW_HEADER_TYPE_Handshake_Command              = 0x06,
	CNGW_HEADER_TYPE_Handshake_Response             = 0x07,
	CNGW_HEADER_TYPE_Firmware_Update_Command        = 0x08,
	CNGW_HEADER_TYPE_Status_Update_Command          = 0x09,
	CNGW_HEADER_TYPE_Log_Command                    = 0x0A,
	CNGW_HEADER_TYPE_Ota_Command                    = 0x0B,
	CNGW_HEADER_TYPE_Device_Report                  = 0x0C,
	CNGW_HEADER_TYPE_Control_Command                = 0x0D,
	CNGW_HEADER_TYPE_End_Marker,

} __attribute__((packed)) CNGW_Header_Type;
/**
 * @brief The bit layout for the firmware version.
 */
typedef struct CNGW_Firmware_Version_t
{
    uint8_t   major     : 8;
    uint8_t   minor     : 8;
    uint32_t  ci        : 29;
    uint8_t   branch_id : 3;
} __attribute__((packed)) CNGW_Firmware_Version_t;


/**
 * @brief Message Header structure used for all communication
 */
typedef struct CNGW_Message_Header_t
{
    CNGW_Header_Type    command_type;

    /**@brief
     * The CNGW_Message_Header_t::data_size is in big endian format.
     * The value should be read and written to using the macros
     * *) CNGW_SET_HEADER_DATA_SIZE
     * *) CNGW_GET_HEADER_DATA_SIZE
     *
     * The sum of bytes that make up the message
     * that sent with header.
     * Some message have variable lengths, the structures
     * are used to define upper memory boundaries.
     * If a structure is of 128 byte, but only 30 bytes
     * were filled into this structure then this value is 30.
     * This can occour when messages that have variable lengths.
     */
    uint16_t            data_size;
    uint8_t             crc;

} __attribute__((packed)) CNGW_Message_Header_t;

/**
 * @brief Structure for the handshake GW1 response message from Gateway to Control MCU
 *
 * This message includes information about the gateway (serial number, firmware version, etc) as well as the
 * response to the challenge which was received in the #CNGW_Handshake_CMD_CN1.  THe response should be generated using
 * the ATMEL chip's #ATMEL_Challenge_MAC function
 *
 * Message is 93 bytes in length (not including header)
 *
 * @note This is send under the header type #CNGW_HEADER_TYPE_Handshake_Response
 */
typedef struct CNGW_Handshake_GW1_t
{
	CNGW_Handshake_Command  command;                                                /** @brief Handshake command number. Should be #CNGW_Handshake_CMD_GW1 */
	CNGW_Handshake_Status   status;                                                 /** @brief handshaking status (status of processing the CN1/GW1 message */
	uint8_t                 gateway_serial[CNGW_SERIAL_NUMBER_LENGTH];              /** @brief ATMEL Serial Number of the gateway */
	uint16_t                gateway_model;                                          /** @brief model number of the gateway */
	CNGW_Firmware_Version_t firmware_version;                                       /** @brief The gateway firmware version*/
	CNGW_Firmware_Version_t bootloader_version;                                     /** @brief The gateway bootloader version*/
	uint32_t                reserved;                                               /** @brief Not used, can be left 0x00000000 */
	uint8_t                 challenge_response[CNGW_CHALLENGE_RESPONSE_LENGTH];     /** @brief Response to the challenge sent in the CN1 message */
	uint8_t                 hmac[CNGW_HMAC_LENGTH];                                 /** @brief HMAC of this entire structure (excluding the HMAC itself */
} __attribute__((packed)) CNGW_Handshake_GW1_t;



/**
 * @brief The frame for CNGW_Handshake_GW1_t
 */
typedef struct CNGW_Handshake_GW1_Frame_t
{
    CNGW_Message_Header_t   header;
    CNGW_Handshake_GW1_t    message;
} __attribute__((packed)) CNGW_Handshake_GW1_Frame_t;



