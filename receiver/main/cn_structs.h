#include "enums.h"
typedef enum CN_BOOL
{
    CN_FALSE    = 0,
    CN_TRUE     = 1
} CN_BOOL;
typedef enum CENSE_SPI_Mode{
	CENSE_SPI_MODE_MASTER,
	CENSE_SPI_MODE_SLAVE
} CENSE_SPI_Mode;

typedef enum CENSE_SPI_Direction {

	SPI_DIRECTION__RECEIVE,
	SPI_DIRECTION__TRANSMIT,
	SPI_DIRECTION__TRANSMIT_RECEIVE

}CENSE_SPI_Direction;

typedef struct Buffer_t
{
    uint8_t volatile *buffer;
    uint16_t volatile size;
    uint16_t maxSize;
} Buffer_t;

typedef struct CENSE_SPI_RX_Positioning_t {
    /** The start position of the latest DMA RX transfer */
	uint16_t volatile 	dma_rx_pos;
	/** If true then the dma rx buffer has wrapped around */
	CN_BOOL  volatile	dma_rx_partial_complete;
//	uint16_t volatile  rx_received_byte_count;
} CENSE_SPI_RX_Positioning_t;

typedef struct 
{
	enum CENSE_SPI_Mode mode;
	//SPI_HandleTypeDef *           hspi;
	//DMA_HandleTypeDef *         hdma_rx;
	//DMA_HandleTypeDef *         hdma_tx;
	enum CENSE_SPI_Direction    direction;
	//CN_BOOL volatile            busy;
	volatile struct Buffer_t    pRXBuffer;
	volatile struct Buffer_t    pTXBuffer;
    /** Used by SPI for keeping track of RX data received */
    CENSE_SPI_RX_Positioning_t  rx_position;
    /** Used by SPI  tracking status of backward data */
	//enum CENSE_SPIMASTER_Query_Data_Status volatile master_query_status;

	//SS_Pin_Function drive_ss_pin_callback; /** Function pointer to drive the proper slave select pin(s) */
	//SS_Pin_Function release_ss_pin_callback; /** Function pointer to release the slave select pin(s) */
	//Response_Ready_IRQ_Enable_Disable_Function response_ready_irq_enable_disable_callback; /** Function pointer to enable / disable IRQ for BDR (Backward Data Ready Pin) */
} CENSE_SPI_MASTER_DataStructure_t;


