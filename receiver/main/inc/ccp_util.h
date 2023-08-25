#ifndef CCP_UTIL_H
#define CCP_UTIL_H


#include <stdlib.h>
#include <stdint.h>

#define SECOND_SINCE_1970		((uint32_t)(1522340061))
#define	MAJOR_VER				0
#define	MINOR_VER				0
#define CI_BLD_NUM				0
#define GIT_BRCH_ID				4


uint8_t CCP_UTIL_Get_Crc8(uint8_t crc, const uint8_t *buffer, uint16_t size);
#endif