/*
 * def.h
 *
 *  Created on: oct 27, 2024
 *      Author: admin
 */

#ifndef U_DEF_H_
#define U_DEF_H_

#include <stdint.h>

/* size = 16 bytes */
typedef struct __attribute__((packed))
{
	uint8_t toDevice;
	uint8_t dlc;
	uint8_t reserved[2];
	uint32_t id;
	uint8_t data[8];
}USB_CAN_Msg_TypeDef;


#endif /* U_DEF_H_ */
