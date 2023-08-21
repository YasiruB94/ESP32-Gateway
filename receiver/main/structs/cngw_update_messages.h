/* ****************************************************************************
 * --- Helper types for the various messages below ---
 ******************************************************************************/
/**@brief This is the number of supported attribute CN
 * has. GW already support more attributes.
 * These values must be sequential in order
 * as it's directly casted from this enum
 * to the enum use on GW.
 * Never include derived attribute in here
 * attribute such as ON is derived from brightness
 * and auto added on the GW when it sees brightness attribute*/
typedef enum CNGW_Channel_Attribute_Type
{
    CNGW_ATTRIBUTE_Start_Marker = 0,
    CNGW_ATTRIBUTE_Brightness   = 0x01,

    CNGW_ATTRIBUTE_End_Marker

} __attribute__((packed)) CNGW_Channel_Attribute_Type;


/**
 * @brief The command for each message
 */
typedef enum CNGW_Update_Command
{
    CNGW_UPDATE_CMD_Attribute   = 0x01,
    CNGW_UPDATE_CMD_Status      = 0x02,

} __attribute__((packed)) CNGW_Update_Command;

/**
 * @brief The status associated with a channel.
 *        Values are bit position into a 32bit value
 */
typedef enum CNGW_Update_Channel_Status
{
    CNGW_UPDATE_CHANNEL_STATUS_CNDR_FAULT_TYPE_START_MARKER     = 1,

    /**@brief All values within CNGW_UPDATE_CHANNEL_STATUS_CNDR_FAULT_TYPE_XX_MARKER
     *        map directly to CNDR_FAULT_Type*/
    CNGW_UPDATE_CHANNEL_STATUS_Bit_Pos_Light_Failure            = CNGW_UPDATE_CHANNEL_STATUS_CNDR_FAULT_TYPE_START_MARKER, /**@brief One or more LEDs on the channel has failed.*/
    CNGW_UPDATE_CHANNEL_STATUS_Bit_Pos_Short_Circuit            = 2,
    CNGW_UPDATE_CHANNEL_STATUS_Bit_Pos_Open_Circuit             = 3,
    CNGW_UPDATE_CHANNEL_STATUS_Bit_Pos_Temperature_Derate       = 4,
    CNGW_UPDATE_CHANNEL_STATUS_Bit_Pos_Temperature_Shotdown     = 5,
    CNGW_UPDATE_CHANNEL_STATUS_Bit_Pos_Volatage_Uncalibrated    = 6, /**@brief No lights installed when driver booted*/

    CNGW_UPDATE_CHANNEL_STATUS_CNDR_FAULT_TYPE_END_MARKER       = 7,

    /**@brief CNGW channel status values which have no relationship to CNDR_FAULT_Type*/
    CNGW_UPDATE_CHANNEL_STATUS_START_MARKER                     = CNGW_UPDATE_CHANNEL_STATUS_CNDR_FAULT_TYPE_END_MARKER,
    CNGW_UPDATE_CHANNEL_STATUS_Bit_Pos_Offline                  = CNGW_UPDATE_CHANNEL_STATUS_START_MARKER, /**@brief Channel Explicitly offline*/
    CNGW_UPDATE_CHANNEL_STATUS_END_MARKER

} __attribute__((packed)) CNGW_Update_Channel_Status;

/* ****************************************************************************
 * --- Messages ---
 ******************************************************************************/
/**
 * @brief An attribute update message for a attribute
 *        per channel.
 */
typedef struct CNGW_Update_Attribute_Message_t
{
    CNGW_Update_Command             command_type; /**@brief Always set to CNGW_UPDATE_CMD_Attribute*/
    CNGW_Address_t                  address;
    CNGW_Channel_Attribute_Type     attribute;
    uint16_t                        value;
    uint8_t                         crc;
} __attribute__((packed)) CNGW_Update_Attribute_Message_t;

/**
 * @brief An attribute update message for a attribute
 *        per channel.
 */
typedef struct CNGW_Update_Channel_Status_Message_t
{
    CNGW_Update_Command             command_type; /**@brief Always set to CNGW_UPDATE_CMD_Status*/
    CNGW_Address_t                  address;

    /**@brief Bit mask of the status using CNGW_Update_Channel_Status. Value of 0 indicate no faults.
     * The Gateway will always directly write out this value to it's database.
     * It does not bit mask it. Thus CN must always send the entire mask each time.*/
    uint32_t                        status_mask;
    uint8_t                         crc;
} __attribute__((packed)) CNGW_Update_Channel_Status_Message_t;

/* ****************************************************************************
 * --- Frames ---
 ******************************************************************************/
/**
 * @brief The frame for CNGW_Update_Attribute_Message_t
 */
typedef struct CNGW_Update_Attribute_Frame_t
{
    CNGW_Message_Header_t           header;
    CNGW_Update_Attribute_Message_t message;
} __attribute__((packed)) CNGW_Update_Attribute_Frame_t;

/**
 * @brief The frame for CNGW_Update_Channel_Status_Message_t
 */
typedef struct CNGW_Update_Channel_Status_Frame_t
{
    CNGW_Message_Header_t                   header;
    CNGW_Update_Channel_Status_Message_t    message;
} __attribute__((packed)) CNGW_Update_Channel_Status_Frame_t;