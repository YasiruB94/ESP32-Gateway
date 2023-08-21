#include "inc/action.h"

esp_err_t Send_Action_Msg_On_Off(uint16_t cabinet, uint16_t address, CNGW_Action_Command cmd, uint16_t level)
{
    CCP_TX_FRAMES_t lmcontrol_res;
    CNGW_Action_Frame_t actionFrame;
    CCP_UTIL_Get_Msg_Header(&actionFrame.header, CNGW_HEADER_TYPE_Action_Commmand, sizeof(actionFrame.message));
    actionFrame.message.command = cmd;
    actionFrame.message.address.target_cabinet = cabinet;
    actionFrame.message.address.address_type = CNGW_ADDRESS_TYPE_Cense_Channel;
    actionFrame.message.address.target_address = address;
    actionFrame.message.action_parameters.fade_unit = CNGW_FADE_UNIT_Milliseconds;
    actionFrame.message.action_parameters.fade_time = 100;
    actionFrame.message.action_parameters.light_level = level;
    actionFrame.message.action_parameters.reserved = 0;
    actionFrame.message.scene_parameters.reserved = 64;
    actionFrame.message.crc = 30; 
    lmcontrol_res.action = &actionFrame;
    consume_GW_message((uint8_t *)&lmcontrol_res);
    return ESP_OK;
}

esp_err_t Send_Action_Msg_broadcast_On_Off(uint16_t cabinet, uint16_t address, CNGW_Action_Command cmd, uint16_t level)
{
    CCP_TX_FRAMES_t lmcontrol_res;
    CNGW_Action_Frame_t actionFrame;
    CCP_UTIL_Get_Msg_Header(&actionFrame.header, CNGW_HEADER_TYPE_Action_Commmand, sizeof(actionFrame.message));
    actionFrame.message.command = cmd;
    actionFrame.message.address.target_cabinet = cabinet;
    actionFrame.message.address.address_type = CNGW_ADDRESS_TYPE_Cense_Broadcast;
    actionFrame.message.address.target_address = address;
    actionFrame.message.action_parameters.fade_unit = CNGW_FADE_UNIT_Milliseconds;
    actionFrame.message.action_parameters.fade_time = 100;
    actionFrame.message.action_parameters.light_level = level;
    actionFrame.message.action_parameters.reserved = 0;
    actionFrame.message.scene_parameters.reserved = 64;
    actionFrame.message.crc = 30; 
    lmcontrol_res.action = &actionFrame;
    consume_GW_message((uint8_t *)&lmcontrol_res);
    return ESP_OK;
}

