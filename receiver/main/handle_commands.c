#include "inc/handle_commands.h"
#include "inc/logging.h"
#include "inc/ccp_util.h"
static const char *TAG = "handle_commands";

void printBits(uint32_t num)
{
    for (int i = 31; i >= 0; i--)
    {
        uint32_t mask = 1u << i;
        printf("%u", (num & mask) ? 1 : 0);

        if (i % 4 == 0)
            printf(" "); // Print a space every 4 bits for better readability
    }
    printf("\n");
}

void Parse_1_Frame(uint8_t *packet, size_t dataSize)
{

    CNGW_Message_Header_t header;
    if (dataSize >= sizeof(CNGW_Message_Header_t))
    {
        memcpy(&header, packet, sizeof(CNGW_Message_Header_t));
        // printf("Command Type: %d\n", header.command_type);
        // printf("Data Size: %d\n", header.data_size);
        // printf("CRC: %d\n", header.crc);
        if (!Is_Header_Valid(&header))
        {
            ESP_LOGE(TAG, "header not valid");
        }

        switch (header.command_type)
        {
        case CNGW_HEADER_TYPE_Query_Command:
        {
            ESP_LOGE(TAG, "message type: CNGW_HEADER_TYPE_Query_Command");
           // const CNGW_Query_Command command = *((CNGW_Query_Command *)packet);

            //  CNGW_Query_Message_Frame_t frame;
            //  memcpy(&frame, packet, sizeof(frame));
            //  ESP_LOGE(TAG, "frame->command: %d", frame.message.command);
            //  ESP_LOGE(TAG, "frame->crc: %d", frame.message.crc);
        }
        break;
        case CNGW_HEADER_TYPE_Status_Update_Command:
        {
            CNGW_Update_Channel_Status_Frame_t frame;
            memcpy(&frame, packet, sizeof(frame));
            if (frame.message.command_type == 2)
            { // status
                ESP_LOGE(TAG, "message type: CNGW_HEADER_TYPE_Status_Update_Command STATUS");
                ESP_LOGI(TAG, "command_type: %d", frame.message.command_type);
                ESP_LOGI(TAG, "address.target_cabinet: %d", frame.message.address.target_cabinet);
                ESP_LOGI(TAG, "address.address_type: %d", frame.message.address.address_type);
                ESP_LOGI(TAG, "address.target_address: %d", frame.message.address.target_address);
                ESP_LOGI(TAG, "status_mask: %u", frame.message.status_mask); // bitwise
                // printBits(frame.message.status_mask);
                ESP_LOGI(TAG, "crc: %u", frame.message.crc);
            }
            else
            { // attribute
                CNGW_Update_Attribute_Frame_t frame2;
                memcpy(&frame2, packet, sizeof(frame2));
                ESP_LOGE(TAG, "message type: CNGW_HEADER_TYPE_Status_Update_Command ATTRIBUTE");
                ESP_LOGI(TAG, "command_type: %d", frame2.message.command_type);
                ESP_LOGI(TAG, "address.target_cabinet: %d", frame2.message.address.target_cabinet);
                ESP_LOGI(TAG, "address.address_type: %d", frame2.message.address.address_type);
                ESP_LOGI(TAG, "address.target_address: %d", frame2.message.address.target_address);
                ESP_LOGI(TAG, "attribute: %d", frame2.message.attribute);
                ESP_LOGI(TAG, "value: %d", frame2.message.value);
                ESP_LOGI(TAG, "crc: %d", frame2.message.crc);
            }
        }
        break;
        case CNGW_HEADER_TYPE_Ota_Command:
        {
            ESP_LOGE(TAG, "message type: CNGW_HEADER_TYPE_Ota_Command");
        }
        break;
        case CNGW_HEADER_TYPE_Log_Command:
        {
            CNGW_Log_Message_Frame_t frame;
            memcpy(&frame, packet, sizeof(frame));
            switch (frame.message.command)
            {
            case CNGW_LOG_TYPE_ERRCODE:
                Handle_Error_Message(packet);
                ESP_LOGE(TAG, "message type: CNGW_LOG_TYPE_ERRCODE");
                break;
            case CNGW_LOG_TYPE_STRING:
                Handle_Log_Message(packet);
                break;
            default:
            ESP_LOGE(TAG, "message type: unknown");
                break;
            }
        }
        break;
        case CNGW_HEADER_TYPE_Configuration_Command:
        {
            ESP_LOGE(TAG, "message type: CNGW_HEADER_TYPE_Configuration_Command");
        }
        break;
        case CNGW_HEADER_TYPE_Device_Report:
        {
            CNGW_Device_Info_Frame_t frame;
            memcpy(&frame, packet, sizeof(frame));
            ESP_LOGE(TAG, "message type: CNGW_HEADER_TYPE_Device_Report");
            ESP_LOGI(TAG, "frame.message.command: %d", frame.message.command);
            ESP_LOGI(TAG, "frame.message.mcu: %d", frame.message.mcu);
            ESP_LOGI(TAG, "frame.message.serial: %u %u %u %u %u %u %u %u %u", frame.message.serial[0], frame.message.serial[1], frame.message.serial[2], frame.message.serial[3], frame.message.serial[4], frame.message.serial[5], frame.message.serial[6], frame.message.serial[7], frame.message.serial[8]);
            ESP_LOGI(TAG, "frame.message.model: %u", frame.message.model);
            ESP_LOGI(TAG, "frame.message.hardware_version: %u.%u-%u", frame.message.hardware_version.major, frame.message.hardware_version.minor, frame.message.hardware_version.ci);
            ESP_LOGI(TAG, "frame.message.bootloader_version: %u.%u-%u-%u", frame.message.bootloader_version.major, frame.message.bootloader_version.minor, frame.message.bootloader_version.ci, frame.message.bootloader_version.branch_id);
            ESP_LOGI(TAG, "frame.message.application_version: %u.%u-%u-%u", frame.message.application_version.major, frame.message.application_version.minor, frame.message.application_version.ci, frame.message.application_version.branch_id);
            ESP_LOGI(TAG, "frame.message.crc: %u", frame.message.crc);

            if (frame.message.mcu == 0)
            {
                ESP_LOGW(TAG, "HANDSHAKE DONE!");
                xQueueReset(GW_response_queue);
                Handshake_done = true;
            }
        }
        break;
        case CNGW_HEADER_TYPE_Handshake_Command:
        {
            CNGW_Handshake_CN1_Frame_t frame;
            memcpy(&frame, packet, sizeof(frame));
            switch (frame.message.command)
            {
            case CNGW_Handshake_CMD_CN1:
            {
                analyze_CNGW_Handshake_CN1_t(packet, BUFFER, true);
            }
            break;
            case CNGW_Handshake_CMD_CN2:
            {
                analyze_CNGW_Handshake_CN2_t(packet, BUFFER, true);
            }
            break;

            default:
                break;
            }
        }
        break;

        default:
            if (header.command_type < 15 && header.command_type != 0)
            {
                ESP_LOGE(TAG, "###unknown command type: %d", header.command_type);
            }

            break;
        }
    }
}

inline uint8_t Is_Header_Valid(const CNGW_Message_Header_t *const header)
{
    /*Calculate CRC and compare*/
    const size_t crc_size = sizeof(CNGW_Message_Header_t) - sizeof(header->crc);
    const uint8_t crc8 = CCP_UTIL_Get_Crc8(0, (uint8_t *)header, crc_size);

    return (crc8 == header->crc);
}