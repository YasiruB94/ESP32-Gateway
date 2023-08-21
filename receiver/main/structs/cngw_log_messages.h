/* ****************************************************************************
 * --- CNGW_HEADER_TYPE_Log_Command -- CNGW_Debug_Message_t ---
 ******************************************************************************/
typedef enum CNGW_Log_Level
{
    CNGW_LOG_DISABLE    = 0,
    CNGW_LOG_ERROR      = 1, /*Default for all MCUs*/
    CNGW_LOG_WARNING    = 2,
    CNGW_LOG_INFO       = 3,
    CNGW_LOG_DEBUG      = 4,
    CNGW_LOG_VERBOSE    = 5
} __attribute__((packed)) CNGW_Log_Level;

typedef enum CNGW_Log_Command
{
    CNGW_LOG_TYPE_INVALID   = 0,
    CNGW_LOG_TYPE_ERRCODE   = 1,
    CNGW_LOG_TYPE_STRING    = 2,
} __attribute__((packed)) CNGW_Log_Command;

/**
 * @brief Message is used to send ASCI text log messages
 *
 * @note This is send under the header type #CNGW_HEADER_TYPE_Log_Command
 *       When the CNGW_Message_Header_t::data_size parameter is calculated
 *       it must calculate based on the size of string added + the additional
 *       data including the string NULL terminator. This size will vary.
 */
typedef struct CNGW_Log_Message_t
{
    CNGW_Log_Command    command;
    uint8_t             serial[CNGW_SERIAL_NUMBER_LENGTH];  /*Originating serial # for the log message*/
    CNGW_Log_Level      severity;

    /** @note
     * A NULL terminated string with the CRC of
     * this message after the NULL terminator.
     * A string cannot exceed 128 bytes.
     * The string must be NULL terminated so the decoder
     * knows where the string ends.
     *
     * The CRC must always be append directly after the NULL terminator.
     * If a string is 16 bytes then copy 16 bytes, add the NULL terminator in
     * byte 17, then add CRC in byte 18.
     *
     * CRC is computed from the first byte of this struct to string NULL terminator
     * (inclusive of the NULL terminator).
     * That is form \p message_type to \p string_and_crc[NULL_terminator_index]
     * */
    char                string_and_crc[128 + 1];
} __attribute__((packed)) CNGW_Log_Message_t;

/**
 * @brief Frame for the CNGW_Log_Message_Frame_t
 */
typedef struct CNGW_Log_Message_Frame_t
{
    CNGW_Message_Header_t   header;
    CNGW_Log_Message_t      message;
} __attribute__((packed)) CNGW_Log_Message_Frame_t;

/* ****************************************************************************
 * --- CNGW_HEADER_TYPE_Log_Command -- CNGW_Fault_Message_t ---
 ******************************************************************************/
/**
 * @brief Message is used to send error codes to be passed on to AWS.
 *
 * @note This is send under the header type #CNGW_HEADER_TYPE_Log_Command
 */
typedef struct CNGW_Error_Message_t
{
    CNGW_Log_Command    command;
    uint8_t             serial[CNGW_SERIAL_NUMBER_LENGTH];
    uint32_t            code;
    uint8_t             crc;
} __attribute__((packed)) CNGW_Error_Message_t;

/**
 * @brief Frame for the CNGW_Error_Code_Message_t
 */
typedef struct CNGW_Error_Message_Frame_t
{
    CNGW_Message_Header_t   header;
    CNGW_Error_Message_t    message;
} __attribute__((packed)) CNGW_Error_Message_Frame_t;