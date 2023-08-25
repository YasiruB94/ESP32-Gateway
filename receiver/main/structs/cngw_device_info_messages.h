/* ****************************************************************************
 * --- Types ---
 ******************************************************************************/
/**
 * @brief The bit layout for the hardware version.
 */
struct CNGW_Hardware_Version_t
{
    uint8_t   major;
    uint8_t   minor;
    uint16_t  ci;
} __attribute__((packed));
typedef struct CNGW_Hardware_Version_t CNGW_Hardware_Version_t;

/**
 * @brief MCU IDs. These IDs are used
 * to index buffer and in loops.
 * Do not change these
 */
//typedef enum CNGW_Source_MCU
//{
//    CNGW_SOURCE_MCU_CN  = 0,
//    CNGW_SOURCE_MCU_SW  = 1,
//    CNGW_SOURCE_MCU_GW  = 2,

    /*These Driver field have to be linearly aligned
     * Because it's used in a loop count and
     * memory indexing on GW*/
//    CNGW_SOURCE_MCU_DR0 = 3,
//    CNGW_SOURCE_MCU_DR1 = 4,
//    CNGW_SOURCE_MCU_DR2 = 5,
//    CNGW_SOURCE_MCU_DR3 = 6,
//    CNGW_SOURCE_MCU_DR4 = 7,
//    CNGW_SOURCE_MCU_DR5 = 8,
//    CNGW_SOURCE_MCU_DR6 = 9,
//    CNGW_SOURCE_MCU_DR7 = 10,

    /*End marker indicating the last #
     * in the enum. Used for looping*/
//    CNGW_SOURCE_MCU_END_MARKER
//} __attribute__((packed)) CNGW_Source_MCU;

/**@brief The enumeration of commands sent with each message
 * to identify the message*/

typedef enum CNGW_Device_Info_Command
{
    CNGW_DEVINFO_CMD_Invalid    = 0x00,
    CNGW_DEVINFO_CMD_Update     = 0x01,
    CNGW_DEVINFO_CMD_Remove     = 0x02,
} __attribute__((packed)) CNGW_Device_Info_Command;

/* ****************************************************************************
 * --- Messages ---
 ******************************************************************************/
/**
 * @brief A message holding all the device information
 *        for one MCU.
 *        CN send this to GW.
 */
typedef struct CNGW_Device_Info_Update_Message_t
{
    CNGW_Device_Info_Command    command;
    CNGW_Source_MCU             mcu;
    uint8_t                     serial[CNGW_SERIAL_NUMBER_LENGTH];
    uint8_t                     model;
    uint8_t                     git_hash[20];
    CNGW_Hardware_Version_t     hardware_version;
    CNGW_Firmware_Version_t     bootloader_version;
    CNGW_Firmware_Version_t     application_version;
    uint8_t                     crc;
} __attribute__((packed)) CNGW_Device_Info_Update_Message_t;

/**
 * @brief A message indicate the MCU has been removed
 *        CN send this to GW.
 *
 *        When CN notice a driver has been removed
 *        it should wait reasonable time
 *        because a user could be plugging in another
 *        driver in a short amount time.
 *        This should be around 30 second to 1 minute.
 */
typedef struct CNGW_Device_Info_Remove_Message_t
{
    CNGW_Device_Info_Command    command;
    CNGW_Source_MCU             mcu; /**@brief The driver MCU that was removed. Can only be drivers. */
    uint8_t                     crc;
} __attribute__((packed)) CNGW_Device_Info_Remove_Message_t;

/* ****************************************************************************
 * --- Messages ---
 ******************************************************************************/
/**
 * @brief A frame describing CNGW_Device_Info_Message_t
 *        Header command is CNGW_HEADER_TYPE_Device_Report
 */
typedef struct CNGW_Device_Info_Frame_t
{
    CNGW_Message_Header_t               header;
    CNGW_Device_Info_Update_Message_t   message;
} __attribute__((packed)) CNGW_Device_Info_Frame_t;

/**
 * @brief A frame describing CNGW_Device_Info_Remove_Message_t
 *        Header command is CNGW_HEADER_TYPE_Device_Report
 */
typedef struct CNGW_Device_Info_Remove_Frame_t
{
    CNGW_Message_Header_t               header;
    CNGW_Device_Info_Remove_Message_t   message;
} __attribute__((packed)) CNGW_Device_Info_Remove_Frame_t;

