/*
 ============================================================================
 Name        : ImeConfig.c
 Author      : mimi
 Version     : 01.2023
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : odczyt parametrów brokera w pliku konfiguracyjnym
 ============================================================================
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h> 
#include <Ime.h>
#include <ImeInverter.h>
#include <netdb.h>
#include <sys/socket.h>
#include <syslog.h>

#include "ImeConfig.h"
#include "ConfigFile.h"
#include "Logger.h"

struct	ServiceUdp	service; 


struct	ServiceUdp* config_get_service()
{
    return &service;
}


int config_SetLoggerPar( const char *serialno, const char *ip_address, const char *ip_port, const char *refresh_time )
{
    int port = atoi(ip_port);
    uint16_t refreshtime = atoi(refresh_time);
    logger_set_parameters(serialno, ip_address, port );
    ime_inverter_set_refresh_time(refreshtime);
    return IME_RETURN_CODE_OK;
}


int config_SetServicePar( const char *ip_port, const char *startup )
{
    service.logfilename = VAR_LOG_FILE;
    service.port = atoi(ip_port);

    if(service.port<SERVICE_PORT_RANGE_MIN || service.port>SERVICE_PORT_RANGE_MAX){
            syslog(LOG_ERR, "IME Inverter: service port out of range: %i, %i", SERVICE_PORT_RANGE_MIN, SERVICE_PORT_RANGE_MAX);
            return IME_RETURN_ERR_ERROR;
    }
    return IME_RETURN_CODE_OK;
}


int config_ReadFile(void)
{
    int cerr;

    struct Config Lista[IME_CONFIG_KEY_END_LIST];
    
    Lista[CONFIG_KEY_LOGGER_SN].key    = "logger_sn";
    Lista[CONFIG_KEY_LOGGER_IPADD].key = "ip_addr";
    Lista[CONFIG_KEY_LOGGER_PORT].key  = "port";
    Lista[CONFIG_KEY_REFRESH_TIME].key = "refresh_time";
    Lista[CONFIG_KEY_SERVICE_PORT].key = "service_port";
    Lista[CONFIG_KEY_START_PARAM].key  = "service_param";
    ConfigClearValue(&Lista, IME_CONFIG_KEY_END_LIST);
    
    cerr = ConfigFileRead(CONFFILE, Lista, IME_CONFIG_KEY_END_LIST);
    if (cerr)
    {
        syslog(LOG_ERR, "IME Inverter: Unable to open config file");
        return EXIT_FAILURE;
    }
 
    /* Logger parameters 
     */
    config_SetLoggerPar(Lista[CONFIG_KEY_LOGGER_SN].value,
                        Lista[CONFIG_KEY_LOGGER_IPADD].value,
                        Lista[CONFIG_KEY_LOGGER_PORT].value,
                        Lista[CONFIG_KEY_REFRESH_TIME].value);

    /* Service initialize
     * port, refresh time
     */
    cerr = config_SetServicePar( Lista[CONFIG_KEY_SERVICE_PORT].value,
                                 Lista[CONFIG_KEY_START_PARAM].value );

    return cerr;
}
