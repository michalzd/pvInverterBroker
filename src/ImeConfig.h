/*
 ============================================================================
 Name        : ImeConfig.h
 Author      : mimi
 Version     : 01.2023
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


/*
 * lista kluczy
 */
enum IME_CONFIG_KEY{
	CONFIG_KEY_LOGGER_SN = 0,
        CONFIG_KEY_LOGGER_IPADD,
	CONFIG_KEY_LOGGER_PORT,
	CONFIG_KEY_REFRESH_TIME,
	CONFIG_KEY_SERVICE_PORT,
	CONFIG_KEY_START_PARAM, 
	IME_CONFIG_KEY_END_LIST
};

extern struct	ServiceUdp	service;

int config_ReadFile(void);
int config_SetServicePar( const char *ip_port, const char *startup );

#endif /* IMECONFIG_H_ */
