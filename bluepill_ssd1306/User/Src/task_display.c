/*
 * task_display.c
 *
 *  Created on: Oct 27, 2024
 *      Author: admin
 */

//#include <string.h> // memset

#include "spi.h"

#include "cmsis_os.h"
//#include "semphr.h"

#include "../ssd1306/ssd1306.h"
#include "../ssd1306/ssd1306_test.h"
#include "../ssd1306/button.h"
#include "../ssd1306/convert.h"

#include "../Inc/def.h"

/* Private define ------------------------------------------------------------*/
#define DefNbCharIndex 2	// index
#define DefNbCharTimeElapse 5

#define DEF_TIMEOUT_QUEUE_SEND       (5 * portTICK_PERIOD_MS)
#define DEF_TIMEOUT_QUEUE_RECEIVE    (1 * portTICK_PERIOD_MS)
#define DEF_TIMEOUT_SEMAPHORE        (5 * portTICK_PERIOD_MS)

/* Private function prototypes -----------------------------------------------*/
static inline void SPI_Transmit(uint8_t* data, uint16_t len);
static inline void SPI_Delay(uint32_t milis);
static inline void GPIO_WritePinCS(GPIO_PinState PinState);
static inline void GPIO_WritePinDC(GPIO_PinState PinState);
static inline void GPIO_WritePinRESET(GPIO_PinState PinState);

//static inline void EnableCoreSysClock(void);
//static inline uint32_t GetSysCoreClockCount();

/* Variables -----------------------------------------------------------------*/
extern osMessageQId UsbReceiveQueueHandle;


void Task_display(void *argument)
{
	static USB_CAN_Msg_TypeDef usb_msg;

	char cnumber[DefNbCharIndex+1];
	int index = 0;
	StructSpi SPI;

	char ctimenumber[DefNbCharTimeElapse+1];
	uint32_t timeElapse;
	uint32_t current_time;


	// init. SSD1306
	SPI.Delay = &SPI_Delay;
	SPI.Write = &SPI_Transmit;
	SPI.CS    = &GPIO_WritePinCS;
	SPI.DC    = &GPIO_WritePinDC;
	SPI.RES   = &GPIO_WritePinRESET;
	SSD1306_SPI_Initialize(&SPI);

	// display logo prebuild
	SSD1306_Display();

	while(1)
	{
		/* something in queue? */
		if (xQueueReceive(UsbReceiveQueueHandle, &usb_msg, ( TickType_t ) DEF_TIMEOUT_QUEUE_RECEIVE) == pdTRUE)
		{
			//ITM_SendChar(usb_msg.data[2]);

			/* do something */
			usb_msg.data[2] = 0;
		}

		osDelay(2000);

		// test SSD1306
		//for( int i=0; i<11; i++) { ssd1306_testsequence(i); osDelay(1000); }

		// begin time elapse
		current_time = xTaskGetTickCount();


		SSD1306_Clear();

		// default button configuration width=50, height=11, radius=3
		DrawButton(0,0 , "01", 1, (index==0) ? true : false);
		DrawButton(0,12, "02", 1, (index==1) ? true : false);
		DrawButton(0,24, "03", 1, (index==2) ? true : false);
		DrawButton(0,36, "04", 1, (index==3) ? true : false);

		// round + text index
		SSD1306_FillCircle(89, 31, 25, WHITE);
		SSD1306_SetCursor(79, 23);
		SSD1306_SetTextSize(2);
		SSD1306_SetTextColor(BLACK);
		ConvertIntToChar( index, cnumber, DefNbCharIndex);
		SSD1306_Println((const char*) cnumber);

		// display time elapse
		SSD1306_SetCursor(0, 48);
		SSD1306_SetTextSize(2);
		SSD1306_SetTextColor(WHITE);
		ConvertIntToChar(timeElapse, ctimenumber, DefNbCharTimeElapse);
		SSD1306_Println((const char*) ctimenumber);

		SSD1306_Display();

		// inc. index for next button
		index ++;

		// check limit
		if (index == 4) { index = 0; }

		// end time elapse
		timeElapse = (xTaskGetTickCount() - current_time);
		timeElapse *= portTICK_RATE_MS;

	}
}


/* SPI Hardware dependencies */

static inline void SPI_Transmit(uint8_t* data, uint16_t len)
{
	//HAL_SPI_Transmit(hspi, pData, Size, Timeout); // Timeout millisecond
	HAL_SPI_Transmit(&hspi1, data, len, 20);
}

static inline void SPI_Delay(uint32_t milis)
{
	osDelay(milis);
}

/* SPI CS -> PB8 */
static inline void GPIO_WritePinCS(GPIO_PinState PinState)
{
	if (PinState != GPIO_PIN_RESET)
	{
		GPIOB->BSRR = GPIO_PIN_8;
	}
	else
	{
		GPIOB->BSRR = (uint32_t)GPIO_PIN_8 << 16u;
	}
}

/* SPI DC -> PB7 */
static inline void GPIO_WritePinDC(GPIO_PinState PinState)
{
	if (PinState != GPIO_PIN_RESET)
	{
		GPIOB->BSRR = GPIO_PIN_7;
	}
	else
	{
		GPIOB->BSRR = (uint32_t)GPIO_PIN_7 << 16u;
	}
}

/* SPI RESET -> PB6 */
static inline void GPIO_WritePinRESET(GPIO_PinState PinState)
{
	if (PinState != GPIO_PIN_RESET)
	{
		GPIOB->BSRR = GPIO_PIN_6;
	}
	else
	{
		GPIOB->BSRR = (uint32_t)GPIO_PIN_6 << 16u;
	}
}

/* END: SPI Hardware dependencies */


/*EOF*/

