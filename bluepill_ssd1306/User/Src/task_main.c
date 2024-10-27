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




/* Private typedef -----------------------------------------------------------*/

typedef struct __attribute__((packed))
{
	uint32_t id;
	uint8_t dlc;
	uint8_t data[8];
}CAN_Msg_TypeDef;


/* Private define ------------------------------------------------------------*/
#define DEF_TIMEOUT_USB_FRAME        (20 * portTICK_PERIOD_MS)

/* Private macro -------------------------------------------------------------*/


/* Private variables ---------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;

static uint8_t _usb_rxbuf[64] = {0};

extern osMessageQId UsbReceiveQueueHandle;

/* Private function prototypes -----------------------------------------------*/
static void UsbTramsmit(void);


/* Global function prototypes -----------------------------------------------*/
void UsbReceiveCallback(const uint8_t* const pBuf, const uint32_t* const pLen);



/* Code ---------------------------------------------------------------------*/

void UsbReceiveCallback(const uint8_t* const pBuf, const uint32_t* const pLen)
{
	static TickType_t last_time = 0;
	static int index = 0;
	static USB_CAN_Msg_TypeDef msg_from_usb;
	static uint16_t less_counter = 0;

	/* Is it time for vATask() to run? */
	BaseType_t xHigherPriorityTaskWoken = pdFALSE;


	TickType_t time_elapse = xTaskGetTickCount() - last_time;

	/* frame timeout */
	if (time_elapse > DEF_TIMEOUT_USB_FRAME)
	{
		index = 0;

		/* Save current time */
		last_time = xTaskGetTickCount();
	}


	// debug : check if we receive less than sizeof(Message_FromHost_TypeDef)
	if (*pLen < sizeof(USB_CAN_Msg_TypeDef))
	{
		less_counter++;
	}


	for(int i=0; i< *pLen; i++)
	{
		_usb_rxbuf[index++] = pBuf[i];
	}

	if (index >= sizeof(USB_CAN_Msg_TypeDef))
	{
		memcpy( &msg_from_usb, &_usb_rxbuf[0], sizeof(USB_CAN_Msg_TypeDef));

		index -= sizeof(USB_CAN_Msg_TypeDef);

		/* put message in queue */
		xQueueSendFromISR(UsbReceiveQueueHandle, &msg_from_usb, &xHigherPriorityTaskWoken);
	}

	/* Yield if xHigherPriorityTaskWoken is true. */
	portYIELD_FROM_ISR( xHigherPriorityTaskWoken );

}


void Task_main(void *argument)
{

	/* initialize USB_DEVICE */
	MX_USB_DEVICE_Init();

	while(1)
	{
		osDelay(1000);
		HAL_GPIO_TogglePin(LED_BUILTIN_GPIO_Port, LED_BUILTIN_Pin);

		UsbTramsmit();
	}
}


static void UsbTramsmit(void)
{
	static USB_CAN_Msg_TypeDef usb_msg;

	usb_msg.toDevice = 0;
	usb_msg.id = 1;

	usb_msg.dlc = 8;

	usb_msg.data[0] = 0;
	usb_msg.data[1] = 1;
	usb_msg.data[2] = 2;
	usb_msg.data[3] = 3;
	usb_msg.data[4] = 4;
	usb_msg.data[5] = 5;
	usb_msg.data[6] = 6;
	usb_msg.data[7] = 7;

	CDC_Transmit_FS((uint8_t*)&usb_msg, sizeof(usb_msg));
}



/*EOF*/
