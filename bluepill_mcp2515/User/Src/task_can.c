/*
 * task_can.c
 *
 *  Created on: Oct 27, 2024
 *      Author: admin
 */



#include <stdbool.h>

#include "spi.h"

#include "usbd_def.h"
#include "usbd_cdc.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

#include "cmsis_os.h"
//#include "semphr.h"

#include "../Inc/def.h"
#include "../mcp2515/can.h"
#include "../mcp2515/mcp2515.h"

/* Private function prototypes -----------------------------------------------*/
static inline void SPI_Delay(uint32_t millis);
static inline void GPIO_WritePinCS(GPIO_PinState PinState);
static inline void SPI_Transmit(uint8_t data);
static inline uint8_t SPI_Receive(void);



extern osMessageQId UsbReceiveQueueHandle;


void Task_can(void *argument)
{

	USB_CAN_Msg_TypeDef msg;
	struct can_frame canMsg;

	StructSpi mcpSPI;
	mcpSPI.Delay = &SPI_Delay;
	mcpSPI.Read  = &SPI_Receive;
	mcpSPI.Write = &SPI_Transmit;
	mcpSPI.CS    = &GPIO_WritePinCS;

	if (MCP2515_SPI_initialize(&mcpSPI) == false) { Error_Handler(); }

	MCP2515_CAN_initialize();
	MCP2515_SetBitrate(CAN_500KBPS);
	MCP2515_SetNormalMode();

	while(1)
	{
		/* Infinite wait time */
		if (xQueueReceive(UsbReceiveQueueHandle, &msg, ( TickType_t ) portMAX_DELAY) == pdTRUE)
		{
			if (msg.signal == (uint8_t)MsgSignalFromCAN)
			{
				if (MCP2515_ReadMessage(&canMsg) == ERROR_OK)
				{
					// id can |= (CAN_EFF_FLAG | CAN_RTR_FLAG | CAN_ERR_FLAG)

					HAL_GPIO_TogglePin(LED_BUILTIN_GPIO_Port, LED_BUILTIN_Pin);

					msg.id = canMsg.can_id;

					msg.dlc = (canMsg.can_dlc > CAN_MAX_DLEN) ? CAN_MAX_DLEN : canMsg.can_dlc;

					msg.data[0] = canMsg.data[0];
					msg.data[1] = canMsg.data[1];
					msg.data[2] = canMsg.data[2];
					msg.data[3] = canMsg.data[3];
					msg.data[4] = canMsg.data[4];
					msg.data[5] = canMsg.data[5];
					msg.data[6] = canMsg.data[6];
					msg.data[7] = canMsg.data[7];

					/* USB transmit to host */
					CDC_Transmit_FS((uint8_t*)&msg, sizeof(msg));
				}
			}
			else
			{
				/* extended id -> add CAN_EFF_FLAG */
				canMsg.can_id = msg.id | CAN_EFF_FLAG;

				canMsg.can_dlc = msg.dlc;

				for(int i=0; i < 8; i++)
				{
					canMsg.data[i] = msg.data[i];
				}

				MCP2515_WriteMessage(&canMsg);
			}


		}

		//osDelay(2000);
	}
}




/** @addtogroup SPI2_Hardware_dependencies
  * @{
  */

/*! \brief Delay function */
static inline void SPI_Delay(uint32_t millis)
{
	osDelay(millis);
}

/* SPI CS -> PB12 */
static inline void GPIO_WritePinCS(GPIO_PinState PinState)
{
	if (PinState != GPIO_PIN_RESET)
	{
		GPIOB->BSRR = GPIO_PIN_12;
	}
	else
	{
		GPIOB->BSRR = (uint32_t)GPIO_PIN_12 << 16u;
	}
}

static inline void SPI_Transmit(uint8_t data)
{
	HAL_SPI_Transmit(&hspi2, &data, 1, 20);
}

static inline uint8_t SPI_Receive(void)
{
	uint8_t c = 0;

	HAL_SPI_Receive(&hspi2, &c, 1, 20);

	return c;
}


/**
  * @}
  */

/* EOF */
