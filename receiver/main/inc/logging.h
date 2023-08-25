#ifndef LOGGING_H
#define LOGGING_H
#include "app_main.h"
#include "handshake.h"


void Handle_Log_Message(const uint8_t *recvbuf);
void Handle_Error_Message(const uint8_t *recvbuf);

#endif