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

