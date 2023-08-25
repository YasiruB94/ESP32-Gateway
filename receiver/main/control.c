#include "inc/control.h"
#include "inc/ccp_util.h"

esp_err_t send_control_message(CNGW_Control_Command cmd, CNGW_Log_Level lvl, CNGW_Source_MCU MCU)
{
    
    CCP_TX_FRAMES_t lmcontrol_res;
    CNGW_Control_Frame_t controlFrame;
    CCP_UTIL_Get_Msg_Header(&controlFrame.header, CNGW_HEADER_TYPE_Control_Command, sizeof(controlFrame.message));
    controlFrame.message.command = cmd;
    controlFrame.message.level = lvl;
    controlFrame.message.mcu_id = MCU;
    controlFrame.message.crc = CCP_UTIL_Get_Crc8(0, (uint8_t *)&(controlFrame.message), sizeof(controlFrame.message) - sizeof(controlFrame.message.crc));
    lmcontrol_res.action = &controlFrame;
    consume_GW_message((uint8_t *)&lmcontrol_res);
    
    return ESP_OK;
}