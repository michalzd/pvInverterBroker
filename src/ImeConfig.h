/*
 ============================================================================
 Name        : ImeConfig.h
 Author      : mimi
 Version     : 01.2003
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : parametry w pliku konfiguracyjnym
 ============================================================================
 */

/*
 * LoggerConfig.h
 *
 *  Created on: 17 maj 2023
 *      Author: mimi
 */

#ifndef IMECONFIG_H_
#define IMECONFIG_H_

#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>

/*
 * 
 */
#define CONFDIR "/etc/ime/"
#define CONFFILE "/etc/ime/Logger"


#ifdef PARAM_OPENWRT
#define VAR_LOG_FILE "/var/imebroker.log"
#else
#define VAR_LOG_FILE "/var/tmp/imebroker.log"
#endif


struct	ServiceUdp 
{
	uint16_t    port;
	const char *logfilename;
};


struct	SofarLogger 
{
	in_addr_t   address;
	uint16_t    port;
	uint64_t    sn;
	uint16_t    refreshtime;
};

/*
 * lista kluczy
 */
enum IME_CONFIG_KEY{
	SOFAR_CONFIG_KEY_LOGGER_SN = 0,
	SOFAR_CONFIG_KEY_LOGGER_IPADD,
	SOFAR_CONFIG_KEY_LOGGER_PORT,
	SOFAR_CONFIG_KEY_REFRESH_TIME,
	CONFIG_KEY_SERVICE_PORT,
	CONFIG_KEY_START_PARAM, 
	IME_CONFIG_KEY_END_LIST
};

extern struct	ServiceUdp	service;
extern struct	SofarLogger sofarLogger;

int config_ReadFile(void);
int config_SetSofarPar( const char *serialno, const char *ip_address, const char *ip_port, const char *refresh_time );
int config_SetServicePar( const char *ip_port, const char *startup );

#endif /* IMECONFIG_H_ */
