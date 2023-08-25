#ifndef HANDLE_COMMANDS_H
#define HANDLE_COMMANDS_H

#include "app_main.h"
#include "handshake.h"
#include "structs/cngw_log_messages.h"
#include "structs/cngw_query_messages.h"
#include "structs/cngw_device_info_messages.h"
#include "structs/cngw_update_messages.h"

void Parse_1_Frame(uint8_t *packet, size_t dataSize);
inline uint8_t Is_Header_Valid(const CNGW_Message_Header_t *const header);

#endif