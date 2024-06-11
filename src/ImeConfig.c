/*
 ============================================================================
 Name        : ImeConfig.c
 Author      : mimi
 Version     : 01.2003
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

struct	ServiceUdp	service;
struct	SofarLogger sofarLogger;


struct	ServiceUdp* config_get_service()
{
    return &service;
}

struct	SofarLogger* config_get_logger()
{
    return &sofarLogger;
}


int config_SetSofarPar( const char *serialno, const char *ip_address, const char *ip_port, const char *refresh_time )
{
    char *end;
    sofarLogger.sn = strtoll(serialno, &end, 10);
    inet_pton(AF_INET, ip_address, &sofarLogger.address);
    sofarLogger.port = atoi(ip_port);
    sofarLogger.refreshtime = atoi(refresh_time);

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
    
    Lista[SOFAR_CONFIG_KEY_LOGGER_SN].key    = "logger_sn";
    Lista[SOFAR_CONFIG_KEY_LOGGER_IPADD].key = "ip_addr";
    Lista[SOFAR_CONFIG_KEY_LOGGER_PORT].key  = "port";
    Lista[SOFAR_CONFIG_KEY_REFRESH_TIME].key = "refresh_time";
    Lista[CONFIG_KEY_SERVICE_PORT].key = "service_port";
    Lista[CONFIG_KEY_START_PARAM].key  = "service_param";
    ConfigClearValue(&Lista, IME_CONFIG_KEY_END_LIST);
    
    cerr = ConfigFileRead(CONFFILE, Lista, IME_CONFIG_KEY_END_LIST);
    if (cerr)
    {
        syslog(LOG_ERR, "IME Inverter: Unable to open config file");
        return EXIT_FAILURE;
    }
 
    /* Sofar Logger parameters
     * sn connection addres and port,
     */
    cerr = config_SetSofarPar(Lista[SOFAR_CONFIG_KEY_LOGGER_SN].value,
                              Lista[SOFAR_CONFIG_KEY_LOGGER_IPADD].value,
                              Lista[SOFAR_CONFIG_KEY_LOGGER_PORT].value,
                              Lista[SOFAR_CONFIG_KEY_REFRESH_TIME].value);
    if(cerr) return cerr;


    /* Service initialize
     * port, refresh time
     */
    cerr = config_SetServicePar( Lista[CONFIG_KEY_SERVICE_PORT].value,
                                 Lista[CONFIG_KEY_START_PARAM].value );

    return cerr;
}
