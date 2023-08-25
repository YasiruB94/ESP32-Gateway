#ifndef CONTROL_H
#define CONTROL_H
#include "app_main.h"
#include "handshake.h"


esp_err_t send_control_message(CNGW_Control_Command cmd, CNGW_Log_Level lvl, CNGW_Source_MCU MCU);

#endif