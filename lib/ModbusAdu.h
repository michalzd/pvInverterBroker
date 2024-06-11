/*
 ============================================================================
 Name        : ModbusAdu.h
 Author      : mimi
 Version     : 01.2023
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : Modbus TCP/IP Ramka ADU
 ============================================================================
 */

#ifndef MODBUSRTU_H_
#define MODBUSRTU_H_

#include <stdlib.h>
#include <stdint.h>


/*
 * modbus RTU data
 */
struct ModBus_Request
{
	uint8_t  device;
	uint8_t  functioncode;
	uint16_t firstreg;		// Logger register
	uint16_t quantity;		// quantity of register
	uint16_t crc;
};

struct ModBus_Response
{
	uint8_t  device;
	uint8_t  functioncode;
	uint8_t  size;			// byte counter
	uint8_t  *data;			// quantity of register
	uint16_t crc;
};

typedef struct ModBus_Request ModBus_Request_t;
typedef struct ModBus_Response ModBus_Response_t;

uint16_t modbusrequest_crc(const ModBus_Request_t * modbusrequest, uint8_t size);
uint16_t modbusswap(uint16_t w16b);
uint8_t  * modbus_cpydata(void *srcmodbusdata, void *desdata, uint8_t len);
uint16_t * modbus_cpydataswap(void *srcmodbusdata, void *desdata, uint8_t len);



#endif /* MODBUSRTU_H_ */
