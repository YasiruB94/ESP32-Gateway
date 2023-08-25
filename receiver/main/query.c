#include "inc/query.h"
#include "inc/ccp_util.h"

esp_err_t query_all_channel_info()
{
    CCP_TX_FRAMES_t lmcontrol_res;
    CNGW_Query_Message_Frame_t queryFrame;

    
    CCP_UTIL_Get_Msg_Header(&queryFrame.header, CNGW_HEADER_TYPE_Query_Command, sizeof(queryFrame.message));
    queryFrame.message.command = CNGW_QUERY_CMD_Get_All_Channel_Info;
    queryFrame.message.crc = CCP_UTIL_Get_Crc8(0, (uint8_t *)&(queryFrame.message), sizeof(queryFrame.message) - sizeof(queryFrame.message.crc));
    lmcontrol_res.query = &queryFrame;
    consume_GW_message((uint8_t *)&lmcontrol_res);

    return ESP_OK;
}