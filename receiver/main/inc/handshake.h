#ifndef HANDSHAKE_H
#define HANDSHAKE_H
#include "app_main.h"
#include "SPI_comm.h"
#include "structs/cn_structs.h"
#include "structs/cngw_action_structs.h"
#include "structs/cngw_handshake_structs.h"


extern bool Handshake_done;
void analyze_CNGW_Handshake_CN1_t(const uint8_t *recvbuf, int size, bool print_data);
void analyze_CNGW_Handshake_CN2_t(const uint8_t *recvbuf, int size, bool print_data);

void prepare_CN_handshake_01_feedbck(CNGW_Handshake_CN1_Frame_t *cn1);
void prepare_CN_handshake_02_feedbck(CNGW_Handshake_CN2_Frame_t *cn2);
void CCP_UTIL_Get_Msg_Header(CNGW_Message_Header_t *const header, CNGW_Header_Type command, const size_t data_size);
const CNGW_Firmware_Version_t *GWVer_Get_Firmware(void);
const CNGW_Firmware_Version_t *GWVer_Get_Bootloader_Firmware(void);

#endif