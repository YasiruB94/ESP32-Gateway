#include "inc/handshake.h"
#include "ccp_util.c"
static const char *TAG = "handshake";
bool Handshake_done = false;

void analyze_CNGW_Handshake_CN1_t(const uint8_t *recvbuf, int size, bool print_data)
{
    CNGW_Handshake_CN1_Frame_t frame;
    memcpy(&frame, recvbuf, sizeof(frame));

    if (print_data)
    {
        ESP_LOGI(TAG, "frame decoded as CNGW_Handshake_CN1_Frame_t");
        printf("frame.header.command_type: %d\n", frame.header.command_type);
        printf("frame.header.data_size: %u\n", frame.header.data_size);
        printf("frame.header.crc: %u\n", frame.header.crc);
        printf("frame.message.cabinet_number: %u\n", frame.message.cabinet_number);
        printf("frame.message.challenge: ");
        for (int i = 0; i < CNGW_CHALLENGE_RESPONSE_LENGTH; i++)
        {
            printf("%u ", frame.message.challenge[i]);
        }
        printf("\n");
        printf("frame.message.command: %d\n", frame.message.command);
        printf("frame.message.hmac: ");
        for (int i = 0; i < CNGW_HMAC_LENGTH; i++)
        {
            printf("%u ", frame.message.hmac[i]);
        }
        printf("\n");
        printf("frame.message.mainboard_serial: ");
        for (int i = 0; i < CNGW_SERIAL_NUMBER_LENGTH; i++)
        {
            printf("%u ", frame.message.mainboard_serial[i]);
        }
        printf("\n");
        ESP_LOGI(TAG, "--END--");
    }
    prepare_CN_handshake_01_feedbck(&frame);
   
}

void analyze_CNGW_Handshake_CN2_t(const uint8_t *recvbuf, int size, bool print_data)
{
    CNGW_Handshake_CN2_Frame_t frame;
    memcpy(&frame, recvbuf, sizeof(frame));

    if (print_data)
    {
        ESP_LOGI(TAG, "frame decoded as CNGW_Handshake_CN2_Frame_t");
        printf("frame.header.command_type: %d\n", frame.header.command_type);
        printf("frame.header.data_size: %u\n", frame.header.data_size);
        printf("frame.header.crc: %u\n", frame.header.crc);
        printf("frame.message.command: %u\n", frame.message.command);
        printf("frame.message.status: %u\n", frame.message.status);
        printf("frame.message.hmac: ");
        for (int i = 0; i < CNGW_HMAC_LENGTH; i++)
        {
            printf("%u ", frame.message.hmac[i]);
        }
        printf("\n");

        ESP_LOGI(TAG, "--END--");
    }
    prepare_CN_handshake_02_feedbck(&frame);
}



void prepare_CN_handshake_01_feedbck(CNGW_Handshake_CN1_Frame_t *cn)
{
    ESP_LOGI(TAG, "prepare_CN_handshake_01_feedback");
    const CNGW_Handshake_CN1_t *const cn1 = &cn->message;
    CNGW_Handshake_Status status = CNGW_Handshake_STATUS_SUCCESS;
    CNGW_Handshake_GW1_Frame_t gw1 = {0};
    // copy the received challenge to the output challenge
    memcpy(gw1.message.challenge_response, &cn1->challenge, sizeof(gw1.message.challenge_response));
    // create the header
    CCP_UTIL_Get_Msg_Header(&gw1.header, CNGW_HEADER_TYPE_Handshake_Response, sizeof(gw1.message));
    // setup the message field
    gw1.message.command = CNGW_Handshake_CMD_GW1; // CNGW_Handshake_CMD_GW1;
    gw1.message.status = status;
    // create dummy data about device serial
    uint8_t gatewaySerial[CNGW_SERIAL_NUMBER_LENGTH] = {'A', 'B', 'C', '1', '2', '3', '4', '5', '6'};
    memcpy(gw1.message.gateway_serial, gatewaySerial, CNGW_SERIAL_NUMBER_LENGTH);
    gw1.message.gateway_model = 0;
    gw1.message.firmware_version = *GWVer_Get_Firmware();
    gw1.message.bootloader_version = *GWVer_Get_Bootloader_Firmware();
    // copy the hmac of the cn mcu to the response hmac
    memcpy(gw1.message.hmac, &cn1->hmac, sizeof(gw1.message.hmac));
    // sending the frame
    consume_GW_message((uint8_t *)&gw1);
    Handshake_done = false;
}

void prepare_CN_handshake_02_feedbck(CNGW_Handshake_CN2_Frame_t *cn)
{
    ESP_LOGI(TAG, "prepare_CN_handshake_02_feedback");
    const CNGW_Handshake_CN2_t *const cn2 = &cn->message;
    CNGW_Handshake_Status status = CNGW_Handshake_STATUS_SUCCESS;
    CNGW_Handshake_GW2_Frame_t gw2 = {0};
    CCP_UTIL_Get_Msg_Header(&gw2.header, CNGW_HEADER_TYPE_Handshake_Response, sizeof(gw2.message));
    // setup the message field
    gw2.message.command = CNGW_Handshake_CMD_GW2;
    gw2.message.status = status;
    memcpy(gw2.message.hmac, &cn2->hmac, sizeof(gw2.message.hmac));
    // sending the frame
    consume_GW_message((uint8_t *)&gw2);
    
}


void CCP_UTIL_Get_Msg_Header(CNGW_Message_Header_t *const header, CNGW_Header_Type command, const size_t data_size)
{
    size_t crc_size = sizeof(*header) - sizeof(header->crc);

    header->command_type = command;
    CNGW_SET_HEADER_DATA_SIZE(header, data_size);
    header->crc = CCP_UTIL_Get_Crc8(0, (uint8_t *)header, crc_size);
}

static const CNGW_Firmware_Version_t current_firmware_version =
    {
        .major = MAJOR_VER,
        .minor = MINOR_VER,
        .ci = CI_BLD_NUM,
        .branch_id = GIT_BRCH_ID};

const CNGW_Firmware_Version_t *GWVer_Get_Firmware(void)
{
    return &current_firmware_version;
}
const CNGW_Firmware_Version_t *GWVer_Get_Bootloader_Firmware(void)
{
    return &current_firmware_version;
}