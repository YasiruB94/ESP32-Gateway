#include "inc/temp.h"
#include "inc/I2C_comm.h"
#include "mbedtls/sha256.h"
#include "include/atecc508a.h"
 #define ATMEL_HMAC_SIZE                     (32u)
static const char *TAG = "temp";



GW_STATUS ATMEL_HMAC(const uint8_t *const message, const uint32_t length, const uint8_t key_slot, uint8_t *hmac, ATMEL_CALLER_CTX_t *const caller_ctx){
    ESP_LOGI(TAG, "ATMEL_HMAC");
    GW_STATUS status = GW_STATUS_GENERIC_TIMEOUT;
        if( NULL == hmac )
    {
        hmac = (uint8_t *)(message + length);
    }
    uint8_t digest[ATMEL_HASH_SIZE] = { 0 };
    mbedtls_sha256_context ctx;
    mbedtls_sha256_init(&ctx);
    mbedtls_sha256_starts_ret(&ctx, 0);
    mbedtls_sha256_update_ret(&ctx, message, length);
    mbedtls_sha256_finish_ret(&ctx, digest);

        ATCAPACKET_t packet = { 0 };

        packet.word_address = 0x03;
        packet.opcode       = ATCA_HMAC;
        packet.param1       = HMAC_MODE_SOURCE_FLAG_MATCH;
        packet.param2       = key_slot;
        packet.txsize       = HMAC_COUNT;
        packet.rxsize       = HMAC_RSP_SIZE;

    ATMELUTIL_Calculate_CRC(&packet);
    uint8_t random[35] = {0};
    esp_err_t ret =  atecc508a_random(random, 0);
 
            printf("####ATMEL_HMAC .hmac: ");
        for (int i = 0; i < ATMEL_HMAC_SIZE; i++)
        {
            printf("%u ", random[i]);
        }
        printf("\n");

    

   // status = ATMELUTIL_Transmit_Packet(&packet, caller_ctx);





    //return status;
    return true;
}

/**
 * @brief Transmits a ATCAPacket to the ATMEL508A and checks for the correct response.
 * wait for longer than it has to.
 * @param packet A valid packet
 * @param size Number of bytes in the buffer
 * @param timeout_ticks The max time in RTOS ticks that this method can block for
 * must include the max time to wait for a response.
 * @return @ref GW_STATUS
 */
GW_STATUS ATMELUTIL_Transmit_Packet(ATCAPACKET_t *const packet, ATMEL_CALLER_CTX_t *const caller_ctx)
{
   // ASSERT_DEBUG_RETURN_RELEASE(packet, GW_STATUS_GENERIC_BAD_PARAM);
   // ASSERT_DEBUG_RETURN_RELEASE(sizeof(packet->data) >= packet->rxsize, GW_STATUS_GENERIC_BAD_PARAM);
/*
    ATMEL_508_CTX_t *const  ctx         = ATMELUTIL_Get_Context();
    GW_STATUS            status      = GW_STATUS_GENERIC_TIMEOUT;

    if( pdTRUE == xSemaphoreTakeRecursive(ctx->mutex, caller_ctx->timeout_ticks) )
    {
        extern I2C_CTX_t i2c0_ctx;

        ATMELUTIL_Adjust_Timeout(caller_ctx);

        //Setup for the TXing
        i2c_master_transfer_t transfer =
        {
            .slaveAddress       = I2C_7BIT_ADDRESS,
            .subaddress         = 0,
            .subaddressSize     = 0,
            .data               = (uint8_t *)packet,
            .dataSize           = 1u + packet->txsize
        };

        uint8_t is_timedout = 0;

        do //Try to TX a packet, if sleep tries to wake device
        {

            if( kStatus_Success == I2C_Write_No_Retry(&i2c0_ctx, transfer, caller_ctx->timeout_ticks ) )
            {
                break; //No Wake required
            }

            is_timedout = ATMELUTIL_Adjust_Timeout(caller_ctx);

            if( !is_timedout )
            {
                //Try to wake the device. Blocking mutex call is in here
                Wake_Atmel(caller_ctx);
                is_timedout = ATMELUTIL_Adjust_Timeout(caller_ctx);
            }

        }while( !is_timedout );

        if( is_timedout )
        {
            status = GW_STATUS_ATMEL_TX_TIMEOUT;
            goto GIVE_MUTEX;
        }

        //Setup and wait for the RX response
        memset(packet->data, 0, packet->rxsize);

        transfer.data       = packet->data;
        transfer.dataSize   = packet->rxsize;

        ATMELUTIL_Adjust_Timeout(caller_ctx);
        if( kStatus_Success != I2C_Read(&i2c0_ctx, &transfer, caller_ctx->timeout_ticks) )
        {
            status = GW_STATUS_ATMEL_RX_TIMEOUT;
            goto GIVE_MUTEX;
        }

        status = ATMELUTIL_Error_Check(packet->data);

        if( GW_STATUS_GENERIC_SUCCESS == status )
        {
            if( packet->rxsize == packet->data[0] )
            {
                memmove(packet->data, &(packet->data[1]), packet->rxsize);
            }
            else
            {
                status = GW_STATUS_ATMEL_INVALID_SIZE;
            }
        }

        GIVE_MUTEX:
        xSemaphoreGiveRecursive(ctx->mutex);
    }

    return status;
    */
   return ESP_OK;
}

/**
 * @brief Return the Atmel 508 context
 * @return A static allocated memory space for ATMEL_508_CTX_t
 */
ATMEL_508_CTX_t *ATMELUTIL_Get_Context(void)
{
    static ATMEL_508_CTX_t ctx = { 0 };
    return &ctx;
}



/**
 * @brief Calculates and adds the CRC to the packet.
 * @param[in] packet The packet to calculate CRC for.
 */
void ATMELUTIL_Calculate_CRC(ATCAPACKET_t *const packet)
{
    const uint8_t   length  = packet->txsize - ATCA_CRC_SIZE;
    uint16_t *const crc     = (uint16_t *)(&(packet->txsize) + length);

    /*If this fails that mean memory boundary will get overwritten*/
    if((const uint8_t *const)crc <= (const uint8_t *const )&(packet->data[sizeof(packet->data) - 1u])){

    }else{

    }
    ESP_LOGI(TAG, "&(packet->txsize): %d, length:%d", packet->txsize, length);
    *crc = CRC16_Poly08005(0, &(packet->txsize), length);
}


/**
 * @brief Calculate the crc16 value using 0x8005 polynomial.
 *        This call will never fail unless hardware fails.
 *        Call takes ~60ns per byte.
 *        Benchmark results
 *          1KByte --> 61us
 *          10KByte --> 589us
 *
 * @param crc_seed[in] Initial state of the polynomial. I.e X(0) = crc_seed.
 * @param buffer[in] The buffer
 * @param length[in] The buffer length
 * @return crc16
 */
uint16_t CRC16_Poly08005(const uint16_t seed, const uint8_t *const buffer, const uint32_t length)
{
    uint32_t crc16 = 0;

    vTaskSuspendAll();
    {
        Init_CRC16_8005(seed);
        CRC_WriteData(CRC0, buffer, length);
        crc16 = CRC_Get16bitResult(CRC0);
    }
    if( !xTaskResumeAll() )
    {
      taskYIELD ();
    }

    return crc16;
}

/**
 * @brief Initialize the standard CRC16 polynomial.
 *        The call order for loading the hardware register
 *        highly matter
 * @param seed[in] The input seed to use
 */
static void Init_CRC16_8005(const uint16_t seed)
{
    crc_config_t config;

    config.polynomial = 0x8005;
    config.seed = seed;
    config.reflectIn = true;
    config.reflectOut = true;
    config.complementChecksum = false;
    config.crcBits = kCrcBits16;
    config.crcResult = kCrcIntermediateChecksum;

    CRC_Init(CRC0, &config);
}
void CRC_Init(CRC_Type *base, const crc_config_t *config)
{
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* ungate clock */
    CLOCK_EnableClock(kCLOCK_Crc0);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
    /* configure CRC module and write the seed */
    if (config->crcResult == kCrcFinalChecksum)
    {
        CRC_SetProtocolConfig(base, config);
    }
    else
    {
        CRC_SetRawProtocolConfig(base, config);
    }
}

/*!
 * @brief Enable the clock for specific IP.
 *
 * @param name  Which clock to enable, see \ref clock_ip_name_t.
 */
static inline void CLOCK_EnableClock(clock_ip_name_t name)
{
    uint32_t regAddr = SIM_BASE + CLK_GATE_ABSTRACT_REG_OFFSET((uint32_t)name);
    (*(volatile uint32_t *)regAddr) |= (1U << CLK_GATE_ABSTRACT_BITS_SHIFT((uint32_t)name));
}

/*!
 * @brief Starts intermediate checksum computation.
 *
 * Configures the CRC module for the specified CRC protocol. @n
 * Starts intermediate checksum computation by writing the seed value.
 * @note CRC_Get16bitResult() or CRC_Get32bitResult() return intermediate checksum (raw data register value).
 *
 * @param base CRC peripheral address.
 * @param protocolConfig Pointer to protocol configuration structure.
 */
static void CRC_SetRawProtocolConfig(CRC_Type *base, const crc_config_t *protocolConfig)
{
    crc_module_config_t moduleConfig;
    /* convert protocol to CRC peripheral module configuration, prepare for intermediate checksum */
    moduleConfig.polynomial = protocolConfig->polynomial;
    moduleConfig.seed = protocolConfig->seed;
    moduleConfig.readTranspose =
        kCrcTransposeNone; /* intermediate checksum does no transpose of data register read value */
    moduleConfig.writeTranspose = CRC_GetTransposeTypeFromReflectIn(protocolConfig->reflectIn);
    moduleConfig.complementChecksum = false; /* intermediate checksum does no xor of data register read value */
    moduleConfig.crcBits = protocolConfig->crcBits;

    CRC_ConfigureAndStart(base, &moduleConfig);
}


/*!
 * @brief Starts final checksum computation.
 *
 * Configures the CRC module for the specified CRC protocol. @n
 * Starts final checksum computation by writing the seed value.
 * @note CRC_Get16bitResult() or CRC_Get32bitResult() return final checksum
 *       (output reflection and xor functions are applied).
 *
 * @param base CRC peripheral address.
 * @param protocolConfig Pointer to protocol configuration structure.
 */
static void CRC_SetProtocolConfig(CRC_Type *base, const crc_config_t *protocolConfig)
{
    crc_module_config_t moduleConfig;
    /* convert protocol to CRC peripheral module configuration, prepare for final checksum */
    moduleConfig.polynomial = protocolConfig->polynomial;
    moduleConfig.seed = protocolConfig->seed;
    moduleConfig.readTranspose = CRC_GetTransposeTypeFromReflectOut(protocolConfig->reflectOut);
    moduleConfig.writeTranspose = CRC_GetTransposeTypeFromReflectIn(protocolConfig->reflectIn);
    moduleConfig.complementChecksum = protocolConfig->complementChecksum;
    moduleConfig.crcBits = protocolConfig->crcBits;

    CRC_ConfigureAndStart(base, &moduleConfig);
}
/*!
 * @brief Returns transpose type for CRC protocol reflect out parameter.
 *
 * This functions helps to set readTranspose member of crc_config_t structure. Reflect out is CRC protocol parameter.
 *
 * @param enable True or false for the selected CRC protocol Reflect Out (refout) parameter.
 */
static inline crc_transpose_type_t CRC_GetTransposeTypeFromReflectOut(bool enable)
{
    return ((enable) ? kCrcTransposeBitsAndBytes : kCrcTransposeNone);
}
/*!
 * @brief Returns transpose type for CRC protocol reflect in parameter.
 *
 * This functions helps to set writeTranspose member of crc_config_t structure. Reflect in is CRC protocol parameter.
 *
 * @param enable True or false for the selected CRC protocol Reflect In (refin) parameter.
 */
static inline crc_transpose_type_t CRC_GetTransposeTypeFromReflectIn(bool enable)
{
    return ((enable) ? kCrcTransposeBitsAndBytes : kCrcTransposeBytes);
}


/*!
 * @brief Starts checksum computation.
 *
 * Configures the CRC module for the specified CRC protocol. @n
 * Starts the checksum computation by writing the seed value
 *
 * @param base CRC peripheral address.
 * @param config Pointer to protocol configuration structure.
 */
static void CRC_ConfigureAndStart(CRC_Type *base, const crc_module_config_t *config)
{
    uint32_t crcControl;

    /* pre-compute value for CRC control registger based on user configuraton without WAS field */
    crcControl = 0 | CRC_CTRL_TOT(config->writeTranspose) | CRC_CTRL_TOTR(config->readTranspose) | CRC_CTRL_FXOR(config->complementChecksum) | CRC_CTRL_TCRC(config->crcBits);

    /* make sure the control register is clear - WAS is deasserted, and protocol is set */
    base->CTRL = crcControl;

    /* write polynomial register */
    base->GPOLY = config->polynomial;

    /* write pre-computed control register value along with WAS to start checksum computation */
    base->CTRL = crcControl | CRC_CTRL_WAS(true);

    /* write seed (initial checksum) */
    base->DATA = config->seed;

    /* deassert WAS by writing pre-computed CRC control register value */
    base->CTRL = crcControl;
}


void CRC_WriteData(CRC_Type *base, const uint8_t *data, size_t dataSize)
{
  //ESP_LOGE(TAG, "test");
 //const uint32_t *data32;
    // 8-bit reads and writes till source address is aligned 4 bytes
    /*
    while ((dataSize) && ((uint32_t)data & 3U))
    {
        base->ACCESS8BIT.DATALL = *data;
        data++;
        dataSize--;
    }

    // use 32-bit reads and writes as long as possible 
    data32 = (const uint32_t *)data;
    while (dataSize >= sizeof(uint32_t))
    {
        base->DATA = *data32;
        data32++;
        dataSize -= sizeof(uint32_t);
    }

    data = (const uint8_t *)data32;

    // 8-bit reads and writes till end of data buffer 
    while (dataSize)
    {
        base->ACCESS8BIT.DATALL = *data;
        data++;
        dataSize--;
    }
    */
}

uint16_t CRC_Get16bitResult(CRC_Type *base)
{
    uint32_t retval;
    uint32_t totr; /* type of transpose read bitfield */

    retval = base->DATA;
    totr = (base->CTRL & CRC_CTRL_TOTR_MASK) >> CRC_CTRL_TOTR_SHIFT;

    /* check transpose type to get 16-bit out of 32-bit register */
    if (totr >= 2U)
    {
        /* transpose of bytes for read is set, the result CRC is in CRC_DATA[HU:HL] */
        retval &= 0xFFFF0000U;
        retval = retval >> 16U;
    }
    else
    {
        /* no transpose of bytes for read, the result CRC is in CRC_DATA[LU:LL] */
        retval &= 0x0000FFFFU;
    }
    return (uint16_t)retval;
}