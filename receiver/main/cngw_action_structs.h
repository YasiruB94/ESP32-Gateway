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

typedef enum CNGW_Action_Command
{
	CNGW_ACTION_CMD_No_Action       = 0x00,
	CNGW_ACTION_CMD_On_Full         = 0x01,
	CNGW_ACTION_CMD_On_At_Level_X   = 0x02,
	CNGW_ACTION_CMD_Off             = 0x03,
	CNGW_ACTION_CMD_Execute_Scene   = 0x04,
	CNGW_ACTION_CMD_Delay_Off       = 0x0a,
	CNGW_ACTION_CMD_On_Last_Level   = 0x0b,
	CNGW_ACTION_CMD_Up              = 0x0c,
	CNGW_ACTION_CMD_Down            = 0x0d,
	CNGW_ACTION_CMD_Toggle_On_Off   = 0x0e

} __attribute__((packed)) CNGW_Action_Command;


typedef enum CNGW_Address_Type
{
	CNGW_ADDRESS_TYPE_Cense_Channel     = 0x01, /** Channel Number between 0 and 31 */
	CNGW_ADDRESS_TYPE_Cense_Group       = 0x02, /** Group Number between 1 and 255 */
	CNGW_ADDRESS_TYPE_Cense_Scene       = 0x03, /** Scene Number between 1 and 255 */
	CNGW_ADDRESS_TYPE_Cense_Broadcast   = 0x0f, /** Broadcast all channels */
	CNGW_ADDRESS_TYPE_Dali_Address      = 0x11, /** DALI Short Address between 0 and 63 */
	CNGW_ADDRESS_TYPE_Dali_Group        = 0x12,	/** DALI Group Address */
	CNGW_ADDRESS_TYPE_Dali_Broadcast    = 0x1f	/** DALI Broadcast */
} __attribute__((packed)) CNGW_Address_Type;


/**
 * @brief Represents an address used as part of some messages
 */
typedef struct CNGW_Address_t
{
    uint8_t                 target_cabinet;
    CNGW_Address_Type       address_type;
    uint8_t                 target_address;

} __attribute__((packed)) CNGW_Address_t;


typedef enum CNGW_Fade_Unit
{
	CNGW_FADE_UNIT_Milliseconds = 0x00,
	CNGW_FADE_UNIT_Seconds      = 0x01,
	CNGW_FADE_UNIT_Minutes      = 0x02
} __attribute__((packed)) CNGW_Fade_Unit;


/**
 * @brief Parameters structure for general action commands
 */
typedef struct CNGW_Action_Parameters_t
{
    CNGW_Fade_Unit  fade_unit   :  2;
    uint16_t        fade_time   : 11; /** Fade time between 0 and 2047 */
    uint16_t        light_level : 12; /** Light level between 0 and 2047 */
    uint8_t         reserved    :  7;
}  __attribute__((packed)) CNGW_Action_Parameters_t;


/**
 * @brief Parameters structure for Execute Scene Action Command
 */
typedef struct CNGW_Action_Scene_Parameters_t
{
    uint32_t reserved;
} __attribute__((packed)) CNGW_Action_Scene_Parameters_t;



/**
 * @brief Represents an Action Message between gateway and control MCU
 *
 * All action commands share this same structure.
 *
 * @note This is send under the header type #CNGW_HEADER_TYPE_Action_Commmand
 *
 */
typedef struct CNGW_Action_Message_t
{
    CNGW_Action_Command command;
    CNGW_Address_t      address;

	union
	{
        CNGW_Action_Parameters_t        action_parameters;
        CNGW_Action_Scene_Parameters_t  scene_parameters;
	};
	uint8_t crc;

} __attribute__((packed)) CNGW_Action_Message_t;

/**
 * @brief Action Message packet
 */
typedef struct CNGW_Action_Frame_t
{
    CNGW_Message_Header_t   header;
    CNGW_Action_Message_t   message;

} __attribute__((packed)) CNGW_Action_Frame_t;


/**
 * @brief The TX frames which can be sent to CN
 */
union CCP_TX_FRAMES
{
    /*Frame which are supported*/
    CNGW_Action_Frame_t                     *action;

    /*Access only pointer for ease of use and strictness*/
    const void *const                       raw_data;
    const CNGW_Message_Header_t *const      generic_header;
    uint8_t test;
};
typedef union CCP_TX_FRAMES CCP_TX_FRAMES_t;