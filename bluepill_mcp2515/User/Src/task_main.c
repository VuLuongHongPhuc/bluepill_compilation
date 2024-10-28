/*
 * task_main.c
 *
 *  Created on: Oct 27, 2024
 *      Author: Phuc VU
 *
 *
 */



/* Includes ------------------------------------------------------------------*/
#include "spi.h"

#include "usbd_def.h"
#include "usbd_cdc.h"
#include "usb_device.h"
#include "usbd_cdc_if.h"

#include "cmsis_os.h"
#include "semphr.h"

#include <stdbool.h>
#include "../Inc/def.h"

#include "../mcp2515/can.h"
#include "../mcp2515/mcp2515.h"


/* Private typedef -----------------------------------------------------------*/


/* Private define ------------------------------------------------------------*/
#define DEF_TIMEOUT_QUEUE_SEND       (5 * portTICK_PERIOD_MS)


/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/

extern osMessageQId UsbReceiveQueueHandle;

/* example how to use semaphore */
SemaphoreHandle_t xSemaphore = NULL;
StaticSemaphore_t xSemaphoreBuffer;

/* Private function prototypes -----------------------------------------------*/



/* Global function prototypes -----------------------------------------------*/
void UsbReceiveCallback(const uint8_t* const pBuf, const uint32_t* const pLen);



/* Code ---------------------------------------------------------------------*/

void UsbReceiveCallback(const uint8_t* const pBuf, const uint32_t* const pLen)
{
	static uint16_t incomplete_frame_counter = 0;
	USB_CAN_Msg_TypeDef msg;

	/* Is it time for vATask() to run? */
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;


	if (*pLen == sizeof(USB_CAN_Msg_TypeDef))
	{
		/* copy buffer */
		memcpy( &msg, &pBuf[0], sizeof(USB_CAN_Msg_TypeDef));

		/* put message in queue */
		xQueueSendFromISR(UsbReceiveQueueHandle, &msg, &xHigherPriorityTaskWoken);
	}
	else
	{
		incomplete_frame_counter++;
	}

	/* Yield if xHigherPriorityTaskWoken is true. */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );
}


void Task_main(void *argument)
{
	USB_CAN_Msg_TypeDef msg;


	xSemaphore = xSemaphoreCreateBinaryStatic( &xSemaphoreBuffer );
	configASSERT( xSemaphore );

	/* initialize USB_DEVICE */
	MX_USB_DEVICE_Init();

	//int size = sizeof(USB_CAN_Msg_TypeDef);

	while(1)
	{
		//osDelay(1000);
		//HAL_GPIO_TogglePin(LED_BUILTIN_GPIO_Port, LED_BUILTIN_Pin);

		/* portMAX_DELAY -> Infinite wait time */
		if( xSemaphoreTake( xSemaphore, ( TickType_t ) portMAX_DELAY ) == pdTRUE )
		{

			HAL_GPIO_TogglePin(LED_BUILTIN_GPIO_Port, LED_BUILTIN_Pin);

			/* /INT triggered -> CAN message received ready */

			msg.signal = MsgSignalFromCAN;

			/* put message in queue */
			xQueueSend(UsbReceiveQueueHandle, &msg, ( TickType_t )DEF_TIMEOUT_QUEUE_SEND);
		}

	}
}


/* External interrupt: INT CAN module */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	// check trigger
	if (GPIO_Pin != MCP2515_INT_Pin) {return; }

	//HAL_GPIO_TogglePin(LED_BUILTIN_GPIO_Port, LED_BUILTIN_Pin);

	/* Is it time for vATask() to run? */
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;

	/* Unblock the task by releasing the semaphore. */
	xSemaphoreGiveFromISR( xSemaphore, &xHigherPriorityTaskWoken );

	/* Yield if xHigherPriorityTaskWoken is true. */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}

/*EOF*/
