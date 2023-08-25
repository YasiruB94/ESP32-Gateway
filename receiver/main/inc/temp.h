#ifndef TEMP_H
#define TEMP_H
#include "app_main.h"
#define GET_GW_STATUS_CLASS_START_MARK(LAYER_TAG)           GW_STATUS_CLASS_##LAYER_TAG##_START_MARK
#define ATMEL_HASH_SIZE                     (32u)
#define ATCA_HMAC                       ((uint8_t)0x11)         //!< HMAC command op-code
#define HMAC_MODE_SOURCE_FLAG_MATCH ((uint8_t)0x04)     //!< HMAC mode bit 2: match TempKey.SourceFlag
#define ATCA_CMD_SIZE_MIN       ((uint8_t)7)
#define HMAC_COUNT                  ATCA_CMD_SIZE_MIN   //!< HMAC command packet size
#define ATCA_RSP_SIZE_32            ((uint8_t)35)                       //!< size of response packet containing 32 bytes data
#define HMAC_RSP_SIZE               ATCA_RSP_SIZE_32    //!< response size of HMAC command
#define ATCA_CRC_SIZE               ((uint8_t)2)                        //!< Number of bytes in the command packet CRC
#define CRC_BASE                                 (0x40032000u)
#define SIM_BASE                                 (0x40047000u)
#define CRC0                                     ((CRC_Type *)CRC_BASE)

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL))
#define FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL 0
#endif
#define     __IO    volatile             /*!< Defines 'read / write' permissions */


#define CRC_CTRL_TOT_MASK                        (0xC0000000U)
#define CRC_CTRL_TOT_SHIFT                       (30U)
#define CRC_CTRL_TOT(x)                          (((uint32_t)(((uint32_t)(x)) << CRC_CTRL_TOT_SHIFT)) & CRC_CTRL_TOT_MASK)

#define CRC_CTRL_TOTR_MASK                       (0x30000000U)
#define CRC_CTRL_TOTR_SHIFT                      (28U)
#define CRC_CTRL_TOTR(x)                         (((uint32_t)(((uint32_t)(x)) << CRC_CTRL_TOTR_SHIFT)) & CRC_CTRL_TOTR_MASK)

#define CRC_CTRL_FXOR_MASK                       (0x4000000U)
#define CRC_CTRL_FXOR_SHIFT                      (26U)
#define CRC_CTRL_FXOR(x)                         (((uint32_t)(((uint32_t)(x)) << CRC_CTRL_FXOR_SHIFT)) & CRC_CTRL_FXOR_MASK)

#define CRC_CTRL_TCRC_MASK                       (0x1000000U)
#define CRC_CTRL_TCRC_SHIFT                      (24U)
#define CRC_CTRL_TCRC(x)                         (((uint32_t)(((uint32_t)(x)) << CRC_CTRL_TCRC_SHIFT)) & CRC_CTRL_TCRC_MASK)

#define CRC_CTRL_WAS_MASK                        (0x2000000U)
#define CRC_CTRL_WAS_SHIFT                       (25U)
#define CRC_CTRL_WAS(x)                          (((uint32_t)(((uint32_t)(x)) << CRC_CTRL_WAS_SHIFT)) & CRC_CTRL_WAS_MASK)


#define ASSERT_DEBUG_RETURN_RELEASE(cond, err)      { if( 0 == (cond) ){ ASSERT(0); return (err);} }


/*!
 * @addtogroup CRC_Peripheral_Access_Layer CRC Peripheral Access Layer
 * @{
 */

/** CRC - Register Layout Typedef */
typedef struct {
  union {                                          /* offset: 0x0 */
    struct {                                         /* offset: 0x0 */
      __IO uint16_t DATAL;                             /**< CRC_DATAL register., offset: 0x0 */
      __IO uint16_t DATAH;                             /**< CRC_DATAH register., offset: 0x2 */
    } ACCESS16BIT;
    __IO uint32_t DATA;                              /**< CRC Data register, offset: 0x0 */
    struct {                                         /* offset: 0x0 */
      __IO uint8_t DATALL;                             /**< CRC_DATALL register., offset: 0x0 */
      __IO uint8_t DATALU;                             /**< CRC_DATALU register., offset: 0x1 */
      __IO uint8_t DATAHL;                             /**< CRC_DATAHL register., offset: 0x2 */
      __IO uint8_t DATAHU;                             /**< CRC_DATAHU register., offset: 0x3 */
    } ACCESS8BIT;
  };
  union {                                          /* offset: 0x4 */
    struct {                                         /* offset: 0x4 */
      __IO uint16_t GPOLYL;                            /**< CRC_GPOLYL register., offset: 0x4 */
      __IO uint16_t GPOLYH;                            /**< CRC_GPOLYH register., offset: 0x6 */
    } GPOLY_ACCESS16BIT;
    __IO uint32_t GPOLY;                             /**< CRC Polynomial register, offset: 0x4 */
    struct {                                         /* offset: 0x4 */
      __IO uint8_t GPOLYLL;                            /**< CRC_GPOLYLL register., offset: 0x4 */
      __IO uint8_t GPOLYLU;                            /**< CRC_GPOLYLU register., offset: 0x5 */
      __IO uint8_t GPOLYHL;                            /**< CRC_GPOLYHL register., offset: 0x6 */
      __IO uint8_t GPOLYHU;                            /**< CRC_GPOLYHU register., offset: 0x7 */
    } GPOLY_ACCESS8BIT;
  };
  union {                                          /* offset: 0x8 */
    __IO uint32_t CTRL;                              /**< CRC Control register, offset: 0x8 */
    struct {                                         /* offset: 0x8 */
           uint8_t RESERVED_0[3];
      __IO uint8_t CTRLHU;                             /**< CRC_CTRLHU register., offset: 0xB */
    } CTRL_ACCESS8BIT;
  };
} CRC_Type;

/**@brief Status codes*/
typedef enum GW_STATUS
{
    /******************************************************************************
     * @brief Generic Status Codes
     ******************************************************************************/
    //GW_STATUS_GENERIC_ERROR = GET_GW_STATUS_CLASS_START_MARK(GENERIC),
    GW_STATUS_GENERIC_SUCCESS,
    GW_STATUS_GENERIC_TIMEOUT,
    GW_STATUS_GENERIC_BAD_PARAM,

    /******************************************************************************
     * @brief AWS Status Codes
     ******************************************************************************/
    //GW_STATUS_AWS_TOPIC_ID_ERR = GET_GW_STATUS_CLASS_START_MARK(AWS),
    GW_STATUS_AWS_BUFFER_TOO_SMALL_ERR,
    GW_STATUS_AWS_PB_ERR,
    GW_STATUS_AWS_CERT_ERR,
    GW_STATUS_AWS_CERT_DEVICE_ERR,
    GW_STATUS_AWS_CERT_SIGNER_ERR,

    //ADD_GW_STATUS_ERR_END_MARKER(AWS),

    /****/
    GW_STATUS_AWS_NETWORK_PHYSICAL_LAYER_CONNECTED,                       //!< GW_STATUS_AWS_NETWORK_PHYSICAL_LAYER_CONNECTED
    GW_STATUS_AWS_NETWORK_MANUALLY_DISCONNECTED,                          //!< GW_STATUS_AWS_NETWORK_MANUALLY_DISCONNECTED
    GW_STATUS_AWS_NETWORK_ATTEMPTING_RECONNECT,                           //!< GW_STATUS_AWS_NETWORK_ATTEMPTING_RECONNECT
    GW_STATUS_AWS_NETWORK_RECONNECTED,                                    //!< GW_STATUS_AWS_NETWORK_RECONNECTED
    GW_STATUS_AWS_MQTT_NOTHING_TO_READ,                                   //!< GW_STATUS_AWS_MQTT_NOTHING_TO_READ
    GW_STATUS_AWS_MQTT_CONNACK_CONNECTION_ACCEPTED,                       //!< GW_STATUS_AWS_MQTT_CONNACK_CONNECTION_ACCEPTED
    GW_STATUS_AWS_JSON_MORE,                                              //!< GW_STATUS_AWS_JSON_MORE Indicate the the JSON document has more things to serialize
    GW_STATUS_AWS_JSON_NONE,                                              //!< GW_STATUS_AWS_JSON_NONE No JSON to be serialized

    /******************************************************************************
     * @brief Device Info Status Codes
     ******************************************************************************/
   // GW_STATUS_DEVINFO_INVALID_FIELD_ERR = GET_GW_STATUS_CLASS_START_MARK(DEVINFO),

   // ADD_GW_STATUS_ERR_END_MARKER(DEVINFO),

    /****/
    GW_STATUS_DEVINFO_STATE_UPDATE,    //!< GW_STATUS_DEVINFO_STATE_UPDATE Only state field was set to update on the cache update
    GW_STATUS_DEVINFO_NO_UPDATE,

    /******************************************************************************
     * @brief OTA Status Codes
     ******************************************************************************/
   // GW_STATUS_OTA_ALLOC_ERR = GET_GW_STATUS_CLASS_START_MARK(OTA),
    GW_STATUS_OTA_BIG_BLOCK_ERR,
    GW_STATUS_OTA_FRAME_TYPE_ERR,
    GW_STATUS_OTA_NO_BYTES_EXPECTED_ERR,
    GW_STATUS_OTA_NO_ACTIVE_SESSION_ERR,
    GW_STATUS_OTA_DOWNLOAD_SYNC_ERR,           //!< GW_STATUS_OTA_DOWNLOAD_SYNC_ERR Indicate that the the tx_byte_count != written_bytes.
    GW_STATUS_OTA_SHA256_ERR,
    GW_STATUS_OTA_VALIDATION_TIMEOUT_ERR,
    GW_STATUS_OTA_BINARY_COUNT_ERR,
    GW_STATUS_OTA_ECDSA_SIGNATURE_ERR,
    GW_STATUS_OTA_CN_SERIAL_ERR,
    GW_STATUS_OTA_INVALID_BINARY_TYPE_ERR,
    GW_STATUS_OTA_CRC32_ERR,
    GW_STATUS_OTA_BINARY_SIZE_ERR,             //!<GW_STATUS_OTA_BINARY_SIZE_ERR The SUM of the binary data does not match the amount of data downloaded.
    GW_STATUS_OTA_SAVE_TIMEOUT_ERR,
    GW_STATUS_OTA_SAVE_ERR,
    GW_STATUS_OTA_DOWNLOAD_TIMEOUT_ERR,
    GW_STATUS_OTA_DOWNLOAD_ERR,

   // ADD_GW_STATUS_ERR_END_MARKER(OTA),

    /****/
    GW_STATUS_OTA_SAVE_DONE,
    GW_STATUS_OTA_DOWNLOAD_DONE,
    GW_STATUS_OTA_VALIDATION_DONE,

    /******************************************************************************
     * @brief CCP Status Codes
     ******************************************************************************/
   // ADD_GW_STATUS_ERR_END_MARKER(CCP),

    /****/
    GW_STATUS_CCP_DUMMY_PACKET,
    GW_STATUS_CCP_FRAME_LOCK,

    /******************************************************************************
     * @brief Channel Status Codes
     ******************************************************************************/
   // GW_STATUS_CHNL_VALIDATE_ERR = GET_GW_STATUS_CLASS_START_MARK(CHNL),
    GW_STATUS_CHNL_VALIDATE_NAME_ERR,
    GW_STATUS_CHNL_VALIDATE_ATTR_ERR,
    GW_STATUS_CHNL_MSG_SIZE_ERR,               //!< GW_STATUS_CHNL_MSG_SIZE_ERR The received messages size is not expected
    GW_STATUS_CHNL_MSG_PARAMETER_ERR,          //!< GW_STATUS_CHNL_MSG_PARAMETER_ERR There is a parameter error on the channel that was received from CN
    GW_STATUS_CHNL_CONFIG_NUMBER_HASH_ERR,     //!< GW_STATUS_CHNL_CONFIG_NUMBER_HASH_ERR

   // ADD_GW_STATUS_ERR_END_MARKER(CHNL),

    /****/
    GW_STATUS_CHNL_ENTRY_END,                  //!< GW_STATUS_CHNL_ENTRY_END No more entries exist
    GW_STATUS_CHNL_WAIT_CN_STATUS_MSG,         //!< GW_STATUS_CHNL_WAIT_CN_STATUS_MSG Waiting on CN to send a status message, caller should sleep for a bit when this is received

    /******************************************************************************
     * @brief Atmel Status Codes
     ******************************************************************************/
  //  GW_STATUS_ATMEL_SHA1_ERR = GET_GW_STATUS_CLASS_START_MARK(ATMEL),
    GW_STATUS_ATMEL_SHA256_ERR,
    GW_STATUS_ATMEL_CERT_DECODING_ERR,
    GW_STATUS_ATMEL_CERT_ENCODING_ERR,

   // ADD_GW_STATUS_ERR_END_MARKER(ATMEL),

    /****/
    GW_STATUS_ATMEL_CERT_BUFFER_TOO_SMALL,    //!< GW_STATUS_ATMEL_CERT_BUFFER_TOO_SMALL
    GW_STATUS_ATMEL_CERT_INVALID_DATE,        //!< GW_STATUS_ATMEL_CERT_INVALID_DATE
    GW_STATUS_ATMEL_CERT_UNEXPECTED_ELEM_SIZE,//!< GW_STATUS_ATMEL_CERT_UNEXPECTED_ELEM_SIZE
    GW_STATUS_ATMEL_CERT_ELEM_MISSING,        //!< GW_STATUS_ATMEL_CERT_ELEM_MISSING
    GW_STATUS_ATMEL_CERT_ELEM_OUT_OF_BOUNDS,  //!< GW_STATUS_ATMEL_CERT_ELEM_OUT_OF_BOUNDS
    GW_STATUS_ATMEL_CERT_BAD_CERT,            //!< GW_STATUS_ATMEL_CERT_BAD_CERT
    GW_STATUS_ATMEL_CERT_WRONG_CERT_DEF,      //!< GW_STATUS_ATMEL_CERT_WRONG_CERT_DEF
    GW_STATUS_ATMEL_CERT_VERIFY_FAILED,       //!< GW_STATUS_ATMEL_CERT_VERIFY_FAILED

    /*Try to keep atecc508 error code by off setting them by ATCA_TO_ATMEL_START_MARK*/
    ATCA_TO_ATMEL_START_MARK,
    GW_STATUS_ATMEL_CONFIG_ZONE_LOCKED     = ATCA_TO_ATMEL_START_MARK + 0x01,
    GW_STATUS_ATMEL_DATA_ZONE_LOCKED       = ATCA_TO_ATMEL_START_MARK + 0x02,
    GW_STATUS_ATMEL_WAKE_FAILED            = ATCA_TO_ATMEL_START_MARK + 0xD0,
    GW_STATUS_ATMEL_CHECKMAC_VERIFY_FAILED = ATCA_TO_ATMEL_START_MARK + 0xD1,
    GW_STATUS_ATMEL_PARSE_ERROR            = ATCA_TO_ATMEL_START_MARK + 0xD2,
    GW_STATUS_ATMEL_CRC                    = ATCA_TO_ATMEL_START_MARK + 0xD4,
    GW_STATUS_ATMEL_UNKNOWN                = ATCA_TO_ATMEL_START_MARK + 0xD5,
    GW_STATUS_ATMEL_ECC                    = ATCA_TO_ATMEL_START_MARK + 0xD6,
    GW_STATUS_ATMEL_FUNC_FAIL              = ATCA_TO_ATMEL_START_MARK + 0xE0,
    GW_STATUS_ATMEL_INVALID_ID             = ATCA_TO_ATMEL_START_MARK + 0xE3,
    GW_STATUS_ATMEL_INVALID_SIZE           = ATCA_TO_ATMEL_START_MARK + 0xE4,
    GW_STATUS_ATMEL_BAD_CRC                = ATCA_TO_ATMEL_START_MARK + 0xE5,
    GW_STATUS_ATMEL_RX_FAIL                = ATCA_TO_ATMEL_START_MARK + 0xE6,
    GW_STATUS_ATMEL_RX_NO_RESPONSE         = ATCA_TO_ATMEL_START_MARK + 0xE7,
    GW_STATUS_ATMEL_TX_TIMEOUT             = ATCA_TO_ATMEL_START_MARK + 0xEA,
    GW_STATUS_ATMEL_RX_TIMEOUT             = ATCA_TO_ATMEL_START_MARK + 0xEB,
    GW_STATUS_ATMEL_BAD_OPCODE             = ATCA_TO_ATMEL_START_MARK + 0xF2,
    GW_STATUS_ATMEL_WAKE_SUCCESS           = ATCA_TO_ATMEL_START_MARK + 0xF3,
    GW_STATUS_ATMEL_EXECUTION_ERROR        = ATCA_TO_ATMEL_START_MARK + 0xF4,
    GW_STATUS_ATMEL_NOT_LOCKED             = ATCA_TO_ATMEL_START_MARK + 0xF8,

    /******************************************************************************
     * @brief DDD Status Codes
     ******************************************************************************/
  //  GW_STATUS_DDD_ATTR_DISABLE_ERR = GET_GW_STATUS_CLASS_START_MARK(DDD),         //!< GW_STATUS_DDD_ATTR_DISABLE_ERR Attribute is disabled. Check configuration
    GW_STATUS_DDD_ATTR_DUPLICATE_ERR,                                  //!< GW_STATUS_DDD_ATTR_DUPLICATE_ERR Attribute duplicated within 1 service. Check configuration
    GW_STATUS_DDD_ATTR_UNKNOWN_ERR,                                    //!< GW_STATUS_DDD_ATTR_UNKNOWN_ERR The attribute you are trying to access is unknown to the service.
    GW_STATUS_DDD_SERVICE_UNKNOWN_ERR,                                 //!< GW_STATUS_DDD_SERVICE_UNKNOWN_ERR The service is unknown to the system
    GW_STATUS_DDD_ATTR_VALUE_RANGE_ERR,                                //!< GW_STATUS_DDD_ATTR_VALUE_RANGE_ERR The attribute is not within the min/max range of the attribute
    GW_STATUS_DDD_PAREMETER_ERR,                                       //!< GW_STATUS_DDD_PAREMETER_ERR Input parameter to the function is invalid
    GW_STATUS_DDD_KEY_DUPLICATE_ERR,                                   //!< GW_STATUS_DDD_KEY_DUPLICATE_ERR, Tried to insert the same key more than once.
    GW_STATUS_DDD_RANGE_ERR,                                           //!< GW_STATUS_DDD_RANGE_ERR Min value > max value
    GW_STATUS_DDD_STATE_ERR,                                           //!< GW_STATUS_DDD_STATE_ERR The database is not in the correct state to allow for the method you are trying to use
    GW_STATUS_DDD_NO_ENTRY_ERR,                                        //!< GW_STATUS_DDD_NO_ENTRY_ERR

   // ADD_GW_STATUS_ERR_END_MARKER(DDD),

    /****/
    GW_STATUS_DDD_NO_UPDATE,                                           //!< GW_STATUS_DDD_NO_UPDATE An Update was not required

    /******************************************************************************
     * @brief HKAPP Status Codes
     ******************************************************************************/
 //   GW_STATUS_HKAPP_ACC_LIMIT_ERR = GET_GW_STATUS_CLASS_START_MARK(HKAPP),            //!< GW_STATUS_HKAPP_ACC_LIMIT_ERR There more primary entries than the was configured in the external flash. The "Total Primary Entries" is wrong in flash
    GW_STATUS_HKAPP_ACC_BRDGE_ERR,                                         //!< GW_STATUS_HKAPP_ACC_BRDGE_ERR Bridge initialization error
    GW_STATUS_HKAPP_SERV_LIMIT_ERR,                                        //!< GW_STATUS_HKAPP_SERV_LIMIT_ERR There more service entries than was configured for the primary entry in external flash. The "Adjacent entry count" is wrong in flash
    GW_STATUS_HKAPP_SERV_INVALID_ERR,                                      //!< GW_STATUS_HKAPP_SERV_INVALID_ERR This service is not supported by the firmware
    GW_STATUS_HKAPP_CHARX_INVALID_ERR,                                     //!< GW_STATUS_HKAPP_CHARX_INVALID_ERR This characteristic is not supported by the firmware
    GW_STATUS_HKAPP_CHARX_RANGE_ERR,                                       //!< GW_STATUS_HKAPP_CHARX_RANGE_ERR The min value is > the max value
    GW_STATUS_HKAPP_CHARX_DUPL_ERR,                                        //!< GW_STATUS_HKAPP_CHARX_DUPL_ERR Duplicate attribute were in the entry initialization list
    GW_STATUS_HKAPP_CHARX_MISS_ERR,                                        //!< GW_STATUS_HKAPP_CHARX_MISS_ERR The charx is missing from the configuration it's required for basic operation

   // ADD_GW_STATUS_ERR_END_MARKER(HKAPP),

    /******************************************************************************
     * @brief HI OLED Status Codes
     ******************************************************************************/
 //   GW_STATUS_HIOLED_Y_ERR = GET_GW_STATUS_CLASS_START_MARK(HIOLED),          //!< GW_STATUS_HIOLED_Y_ERR Invalid Y axis parameter
    GW_STATUS_HIOLED_X_ERR,                                        //!< GW_STATUS_HIOLED_X_ERR Invalid X axis parameter
    GW_STATUS_HIOLED_COLOUR_ERR,                                   //!< GW_STATUS_HIOLED_COLOUR_ERR
    GW_STATUS_HIOLED_INDEX_ERR,                                    //!< GW_STATUS_HIOLED_INDEX_ERR

  //  ADD_GW_STATUS_ERR_END_MARKER(HIOLED),

    /******************************************************************************
     * @brief IP STATUS
     ******************************************************************************/
  //  GW_STATUS_IP_SOCK_ERR = GET_GW_STATUS_CLASS_START_MARK(IP),           //!< GW_STATUS_IP_SOCK_ERR Some kind of socket error related to fnet stack. Only option is to close the connection and restablish it
    GW_STATUS_IP_TLS_HANDSHAKE_ERR,                            //!< GW_STATUS_IP_TLS_HANDSHAKE_ERR The Handshake process failed. Further Diagnostic will be required to validate why it failed
    GW_STATUS_IP_GET_DEVICE_PUBLIC_KEY_ERR,
    GW_STATUS_IP_DNS_ERR,                                      //!< GW_STATUS_IP_DNS_ERR The DNS hostname could not be resolved after trying to search

 //   ADD_GW_STATUS_ERR_END_MARKER(IP),

    /****/
    GW_STATUS_IP_TLS_CERT_PARSE_PARTIAL,                        //!< GW_STATUS_IP_TLS_CERT_PARSE_PARTIAL Only some of the certificate that were set to parse succeeded
    GW_STATUS_IP_TLS_NOT_CONNECTED,                             //!< GW_STATUS_IP_TLS_NOT_CONNECTED TLS
    GW_STATUS_IP_NO_IP,                                        //!< GW_STATUS_IP_NO_IP

    /******************************************************************************
     * @brief File System STATUS
     ******************************************************************************/
  //  GW_STATUS_FS_CRC_ERR = GET_GW_STATUS_CLASS_START_MARK(FS),
    GW_STATUS_FS_RECORD_CORRUPT_ERR,

//    ADD_GW_STATUS_ERR_END_MARKER(FS),

    /****/
    GW_STATUS_FS_INVD_SIZE,
    GW_STATUS_FS_FULL,
    GW_STATUS_FS_NOREC,

    /******************************************************************************
     * @brief Interrupt Timer STATUS
     ******************************************************************************/
  //  ADD_GW_STATUS_ERR_END_MARKER(IT),

    /****/
    GW_STATUS_IT_USE_RTOS_DELAY,   //!< GW_STATUS_IT_USE_RTOS_DELAY The delays specified can be done using the RTOS delay method
    GW_STATUS_IT_NOT_TRIGGERED,    //!< GW_STATUS_IT_NOT_TRIGGERED The hardware timer did not trigger in a reasonable time, it was forced stop

    /******************************************************************************
     * @brief Linked List Status
     ******************************************************************************/
  //  ADD_GW_STATUS_ERR_END_MARKER(LL),

    /****/
    GW_STATUS_LL_ITERATOR_NEXT,
    GW_STATUS_LL_ITERATOR_STOP,

    /* *****************************************************************************
     * NO EDITING BELOW HERE
     ******************************************************************************/
    GW_STATUS_END_MARKER                  //!< GW_STATUS_END_MARKER
} GW_STATUS;
#define CLK_GATE_REG_OFFSET_SHIFT 16U
#define CLK_GATE_REG_OFFSET_MASK 0xFFFF0000U
#define CLK_GATE_BIT_SHIFT_SHIFT 0U
#define CLK_GATE_BIT_SHIFT_MASK 0x0000FFFFU

#define CLK_GATE_DEFINE(reg_offset, bit_shift)                                  \
    ((((reg_offset) << CLK_GATE_REG_OFFSET_SHIFT) & CLK_GATE_REG_OFFSET_MASK) | \
     (((bit_shift) << CLK_GATE_BIT_SHIFT_SHIFT) & CLK_GATE_BIT_SHIFT_MASK))

#define CLK_GATE_ABSTRACT_REG_OFFSET(x) (((x)&CLK_GATE_REG_OFFSET_MASK) >> CLK_GATE_REG_OFFSET_SHIFT)
#define CLK_GATE_ABSTRACT_BITS_SHIFT(x) (((x)&CLK_GATE_BIT_SHIFT_MASK) >> CLK_GATE_BIT_SHIFT_SHIFT)
/*! @brief Clock gate name used for CLOCK_EnableClock/CLOCK_DisableClock. */
typedef enum _clock_ip_name
{
    kCLOCK_IpInvalid = 0U,
    kCLOCK_I2c2 = CLK_GATE_DEFINE(0x1028U, 6U),
    kCLOCK_Uart4 = CLK_GATE_DEFINE(0x1028U, 10U),
    kCLOCK_Uart5 = CLK_GATE_DEFINE(0x1028U, 11U),

    kCLOCK_Enet0 = CLK_GATE_DEFINE(0x102CU, 0U),
    kCLOCK_Dac0 = CLK_GATE_DEFINE(0x102CU, 12U),
    kCLOCK_Dac1 = CLK_GATE_DEFINE(0x102CU, 13U),

    kCLOCK_Spi2 = CLK_GATE_DEFINE(0x1030U, 12U),
    kCLOCK_Sdhc0 = CLK_GATE_DEFINE(0x1030U, 17U),
    kCLOCK_Ftm3 = CLK_GATE_DEFINE(0x1030U, 25U),
    kCLOCK_Adc1 = CLK_GATE_DEFINE(0x1030U, 27U),

    kCLOCK_Ewm0 = CLK_GATE_DEFINE(0x1034U, 1U),
    kCLOCK_Cmt0 = CLK_GATE_DEFINE(0x1034U, 2U),
    kCLOCK_I2c0 = CLK_GATE_DEFINE(0x1034U, 6U),
    kCLOCK_I2c1 = CLK_GATE_DEFINE(0x1034U, 7U),
    kCLOCK_Uart0 = CLK_GATE_DEFINE(0x1034U, 10U),
    kCLOCK_Uart1 = CLK_GATE_DEFINE(0x1034U, 11U),
    kCLOCK_Uart2 = CLK_GATE_DEFINE(0x1034U, 12U),
    kCLOCK_Uart3 = CLK_GATE_DEFINE(0x1034U, 13U),
    kCLOCK_Usbfs0 = CLK_GATE_DEFINE(0x1034U, 18U),
    kCLOCK_Cmp0 = CLK_GATE_DEFINE(0x1034U, 19U),
    kCLOCK_Cmp1 = CLK_GATE_DEFINE(0x1034U, 19U),
    kCLOCK_Cmp2 = CLK_GATE_DEFINE(0x1034U, 19U),
    kCLOCK_Vref0 = CLK_GATE_DEFINE(0x1034U, 20U),

    kCLOCK_Lptmr0 = CLK_GATE_DEFINE(0x1038U, 0U),
    kCLOCK_PortA = CLK_GATE_DEFINE(0x1038U, 9U),
    kCLOCK_PortB = CLK_GATE_DEFINE(0x1038U, 10U),
    kCLOCK_PortC = CLK_GATE_DEFINE(0x1038U, 11U),
    kCLOCK_PortD = CLK_GATE_DEFINE(0x1038U, 12U),
    kCLOCK_PortE = CLK_GATE_DEFINE(0x1038U, 13U),

    kCLOCK_Ftf0 = CLK_GATE_DEFINE(0x103CU, 0U),
    kCLOCK_Dmamux0 = CLK_GATE_DEFINE(0x103CU, 1U),
    kCLOCK_Flexcan0 = CLK_GATE_DEFINE(0x103CU, 4U),
    kCLOCK_Rnga0 = CLK_GATE_DEFINE(0x103CU, 9U),
    kCLOCK_Spi0 = CLK_GATE_DEFINE(0x103CU, 12U),
    kCLOCK_Spi1 = CLK_GATE_DEFINE(0x103CU, 13U),
    kCLOCK_Sai0 = CLK_GATE_DEFINE(0x103CU, 15U),
    kCLOCK_Crc0 = CLK_GATE_DEFINE(0x103CU, 18U),
    kCLOCK_Usbdcd0 = CLK_GATE_DEFINE(0x103CU, 21U),
    kCLOCK_Pdb0 = CLK_GATE_DEFINE(0x103CU, 22U),
    kCLOCK_Pit0 = CLK_GATE_DEFINE(0x103CU, 23U),
    kCLOCK_Ftm0 = CLK_GATE_DEFINE(0x103CU, 24U),
    kCLOCK_Ftm1 = CLK_GATE_DEFINE(0x103CU, 25U),
    kCLOCK_Ftm2 = CLK_GATE_DEFINE(0x103CU, 26U),
    kCLOCK_Adc0 = CLK_GATE_DEFINE(0x103CU, 27U),
    kCLOCK_Rtc0 = CLK_GATE_DEFINE(0x103CU, 29U),

    kCLOCK_Flexbus0 = CLK_GATE_DEFINE(0x1040U, 0U),
    kCLOCK_Dma0 = CLK_GATE_DEFINE(0x1040U, 1U),
    kCLOCK_Sysmpu0 = CLK_GATE_DEFINE(0x1040U, 2U),
} clock_ip_name_t;


/**
 * @brief The caller context for atmel api.
 * I.e the public context
 */
struct ATMEL_CALLER_CONTEXT
{
    uint32_t    timeout_ticks;
    TimeOut_t   timer;

    /*On multi-call process we don't want to force sleep
     * the ATECC508*/
    uint8_t     is_sleep;
};
typedef struct ATMEL_CALLER_CONTEXT ATMEL_CALLER_CTX_t;

/**
 * @brief The original ATCA packet struct
 * from the ATCA library.
 * This structure is sent over the wrire
 */
typedef struct ATCAPACKET_t
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

} __attribute__((packed)) ATCAPACKET_t;
/*! @brief CRC bit width */
typedef enum _crc_bits
{
    kCrcBits16 = 0U, /*!< Generate 16-bit CRC code  */
    kCrcBits32 = 1U  /*!< Generate 32-bit CRC code  */
} crc_bits_t;

/*! @brief CRC result type */
typedef enum _crc_result
{
    kCrcFinalChecksum = 0U,       /*!< CRC data register read value is the final checksum.
                                      Reflect out and final xor protocol features are applied. */
    kCrcIntermediateChecksum = 1U /*!< CRC data register read value is intermediate checksum (raw value).
                                      Reflect out and final xor protocol feature are not applied.
                                      Intermediate checksum can be used as a seed for CRC_Init()
                                      to continue adding data to this checksum. */
} crc_result_t;

/*!
* @brief CRC protocol configuration.
*
* This structure holds the configuration for the CRC protocol.
*
*/
typedef struct _crc_config
{
    uint32_t polynomial;     /*!< CRC Polynomial, MSBit first.
                                  Example polynomial: 0x1021 = 1_0000_0010_0001 = x^12+x^5+1 */
    uint32_t seed;           /*!< Starting checksum value */
    bool reflectIn;          /*!< Reflect bits on input. */
    bool reflectOut;         /*!< Reflect bits on output. */
    bool complementChecksum; /*!< True if the result shall be complement of the actual checksum. */
    crc_bits_t crcBits;      /*!< Selects 16- or 32- bit CRC protocol. */
    crc_result_t crcResult;  /*!< Selects final or intermediate checksum return from CRC_Get16bitResult() or
                                CRC_Get32bitResult() */
} crc_config_t;

/*! @brief CRC type of transpose of read write data */
typedef enum _crc_transpose_type
{
    kCrcTransposeNone = 0U,         /*! No transpose  */
    kCrcTransposeBits = 1U,         /*! Tranpose bits in bytes  */
    kCrcTransposeBitsAndBytes = 2U, /*! Transpose bytes and bits in bytes */
    kCrcTransposeBytes = 3U,        /*! Transpose bytes */
} crc_transpose_type_t;

/*!
* @brief CRC module configuration.
*
* This structure holds the configuration for the CRC module.
*/
typedef struct _crc_module_config
{
    uint32_t polynomial;                 /*!< CRC Polynomial, MSBit first.@n
                                              Example polynomial: 0x1021 = 1_0000_0010_0001 = x^12+x^5+1 */
    uint32_t seed;                       /*!< Starting checksum value */
    crc_transpose_type_t readTranspose;  /*!< Type of transpose when reading CRC result. */
    crc_transpose_type_t writeTranspose; /*!< Type of transpose when writing CRC input data. */
    bool complementChecksum;             /*!< True if the result shall be complement of the actual checksum. */
    crc_bits_t crcBits;                  /*!< Selects 16- or 32- bit CRC protocol. */
} crc_module_config_t;

/**
 * @brief The private data context for the ATMEL 508
 * driver
 */
struct ATMEL_508_CONTEXT
{
    SemaphoreHandle_t   mutex;
};
typedef struct ATMEL_508_CONTEXT ATMEL_508_CTX_t;

GW_STATUS ATMEL_HMAC(const uint8_t *const message, const uint32_t length, const uint8_t key_slot, uint8_t *hmac, ATMEL_CALLER_CTX_t *const caller_ctx);
void ATMELUTIL_Calculate_CRC(ATCAPACKET_t *const packet);
uint16_t CRC16_Poly08005(const uint16_t seed, const uint8_t *const buffer, const uint32_t length);
static void Init_CRC16_8005(const uint16_t seed);
void CRC_Init(CRC_Type *base, const crc_config_t *config);
static inline void CLOCK_EnableClock(clock_ip_name_t name);
static void CRC_SetProtocolConfig(CRC_Type *base, const crc_config_t *protocolConfig);
static inline crc_transpose_type_t CRC_GetTransposeTypeFromReflectOut(bool enable);
static inline crc_transpose_type_t CRC_GetTransposeTypeFromReflectIn(bool enable);
static void CRC_ConfigureAndStart(CRC_Type *base, const crc_module_config_t *config);
static void CRC_SetRawProtocolConfig(CRC_Type *base, const crc_config_t *protocolConfig);
void CRC_WriteData(CRC_Type *base, const uint8_t *data, size_t dataSize);
uint16_t CRC_Get16bitResult(CRC_Type *base);
GW_STATUS ATMELUTIL_Transmit_Packet(ATCAPACKET_t *const packet, ATMEL_CALLER_CTX_t *const caller_ctx);
ATMEL_508_CTX_t *ATMELUTIL_Get_Context(void);

#endif