#ifndef ACTION_H
#define ACTION_H
#include "app_main.h"
#include "handshake.h"

esp_err_t Send_Action_Msg_On_Off(uint16_t cabinet, uint16_t address, CNGW_Action_Command cmd, uint16_t level);
esp_err_t Send_Action_Msg_broadcast_On_Off(uint16_t cabinet, uint16_t address, CNGW_Action_Command cmd, uint16_t level);

#endif