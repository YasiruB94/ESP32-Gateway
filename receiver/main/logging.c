#include "inc/logging.h"
#include "inc/ccp_util.h"
static const char *TAG = "logging";

void Handle_Log_Message(const uint8_t *recvbuf)
{
    CNGW_Log_Message_Frame_t frame;
    memcpy(&frame, recvbuf, sizeof(frame));
    CNGW_Log_Command command = frame.message.command;
    uint8_t serial[CNGW_SERIAL_NUMBER_LENGTH];
    memcpy(serial, frame.message.serial, sizeof(frame.message.serial));
    CNGW_Log_Level severity = frame.message.severity;
    char information[128+1];
    memcpy(information, frame.message.string_and_crc, sizeof(frame.message.string_and_crc));
    ESP_LOGI(TAG, "cmd: %d serial: [%d %d %d %d %d %d %d %d %d] severity: %d data: %s", command, serial[0], serial[1], serial[2], serial[3], serial[4], serial[5], serial[6], serial[7], serial[8],severity, information);
    
}

void Handle_Error_Message(const uint8_t *recvbuf)
{
    CNGW_Log_Message_Frame_t frame;
    memcpy(&frame, recvbuf, sizeof(frame));
    CNGW_Log_Command command = frame.message.command;
    uint8_t serial[CNGW_SERIAL_NUMBER_LENGTH];
    memcpy(serial, frame.message.serial, sizeof(frame.message.serial));
    CNGW_Log_Level severity = frame.message.severity;
    char information[128+1];
    memcpy(information, frame.message.string_and_crc, sizeof(frame.message.string_and_crc));
    ESP_LOGI(TAG, "cmd: %d serial: [%d %d %d %d %d %d %d %d %d] severity: %d data: %s", command, serial[0], serial[1], serial[2], serial[3], serial[4], serial[5], serial[6], serial[7], serial[8],severity, information);
    
}