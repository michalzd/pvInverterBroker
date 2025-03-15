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
#include <retcode.h>
#include <threadInverter.h>
#include <netdb.h>
#include <sys/socket.h>
#include <syslog.h>

#include "Service.h"
#include "Config.h"
#include "ConfigFile.h"
#include "Logger.h"

struct	ConfigBroker config; 


int config_SetLoggerPar( const char *serialno, const char *ip_address, const char *ip_port, const char *refresh_time )
{
    int port = atoi(ip_port);
    uint16_t refreshtime = atoi(refresh_time);
    logger_set_parameters(serialno, ip_address, port );
    thread_inverter_set_refresh_time(refreshtime);
    return EXIT_SUCCESS;
}


int config_SetServicePar( const char *ip_port, const char *startup )
{
    config.service.logfilename = VAR_LOG_FILE;
    config.service.port = atoi(ip_port);

    if(config.service.port<SERVICE_PORT_RANGE_MIN || config.service.port>SERVICE_PORT_RANGE_MAX)
    {
        syslog(LOG_ERR, "InverterBroker: service port out of range: %i, %i", SERVICE_PORT_RANGE_MIN, SERVICE_PORT_RANGE_MAX);
        if(print_debug_info) printf("InverterBroker: service port %i out of range", config.service.port);
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}


void config_SetMqttPar( const char *ip_address,  const char *client_id, const char *topic, const char *binary_topic )
{
    char ip_port[5] = "1883"; 
    char *iport;
    
    config.mqtt.port = 0;
    if( strlen(topic)==0 ) return;
    
    iport = strchr(ip_address, ':');
    if(iport)
    {
       *iport = 0;
       strncpy(ip_port, ++iport, 4);
    }
    strncpy(config.mqtt.host, ip_address, MQTT_HOST_SZ);
    config.mqtt.port = atoi(ip_port);
    
    strncpy(config.mqtt.clientid, "10", MQTT_CLIENTID_SZ);
    if(client_id!=NULL && client_id[0]!=0) strncpy(config.mqtt.clientid, client_id, MQTT_CLIENTID_SZ);
     
    // pusty username i password
    config.mqtt.user[0]   = 0;  
    config.mqtt.passwd[0] = 0;
    
    strncpy(config.mqtt.topic, topic, MQTT_TOPIC_SZ);
    config.mqtt.binarytopic[0] = 0;
    if(binary_topic[0] == 0) strncpy(config.mqtt.binarytopic, binary_topic, MQTT_TOPIC_SZ);

}

void config_SetDomoticzPar(const char *topic,  const char *state_id, 
                           const char *pvpower_id, const char *gridpower_idx, const char *consumtonpover_idx,
                           const char *pvstring1_id, const char *pvstring2_id)
{
    config.domoticz.topic[0] = 0;
    config.domoticz.stateidx[0] = 0;
    config.domoticz.pvpoweridx[0] = 0;
    config.domoticz.gridpoweridx[0] = 0;
    config.domoticz.conspoweridx[0] = 0;
    config.domoticz.pvstring1idx[0] = 0;
    config.domoticz.pvstring2idx[0] = 0;
    
    if( strlen(topic)==0 ) return;
    if( strcmp(topic, "none" )==0 ) return;
    
    strncpy(config.domoticz.topic, topic, MQTT_TOPIC_SZ);
    strncpy(config.domoticz.stateidx, state_id, MQTT_DOMOTICZ_ID_SZ);
    strncpy(config.domoticz.pvpoweridx, pvpower_id, MQTT_DOMOTICZ_ID_SZ);
    
    if( strlen(gridpower_idx) ) strncpy(config.domoticz.gridpoweridx, gridpower_idx, MQTT_DOMOTICZ_ID_SZ);
    if( strlen(consumtonpover_idx) ) strncpy(config.domoticz.conspoweridx, consumtonpover_idx, MQTT_DOMOTICZ_ID_SZ);
    if( strlen(pvstring1_id) ) strncpy(config.domoticz.pvstring1idx, pvstring1_id, MQTT_DOMOTICZ_ID_SZ);
    if( strlen(pvstring2_id) ) strncpy(config.domoticz.pvstring2idx, pvstring2_id, MQTT_DOMOTICZ_ID_SZ);
}

int config_ReadFile(void)
{
    int cerr;

    struct Config Lista[CONFIG_KEY_END_LIST];
    
    Lista[CONFIG_KEY_LOGGER_SN].key    = "logger_sn";
    Lista[CONFIG_KEY_LOGGER_IPADD].key = "logger_addr";
    Lista[CONFIG_KEY_LOGGER_PORT].key  = "logger_port";
    Lista[CONFIG_KEY_REFRESH_TIME].key = "refresh_time";
    Lista[CONFIG_KEY_SERVICE_PORT].key = "service_port";
    Lista[CONFIG_KEY_START_PARAM].key  = "service_param";
    
    Lista[CONFIG_KEY_MQTT_SERVER].key   = "mqtt_server";
    Lista[CONFIG_KEY_MQTT_CLIENTID].key = "mqtt_clientid";
    Lista[CONFIG_KEY_MQTT_TOPIC].key    = "mqtt_topic";
    Lista[CONFIG_KEY_MQTT_BINARY_TOPIC].key = "mqtt_binary_topic";
    
    Lista[CONFIG_KEY_DOMOTICZ_TOPIC].key         = "domoticz_topic";
    
    Lista[CONFIG_KEY_DOMOTICZ_STATE_ID].key = "domoticz_inverter_state";
    Lista[CONFIG_KEY_DOMOTICZ_PV_POWER_ID].key   = "domoticz_pvpower_idx";
    Lista[CONFIG_KEY_DOMOTICZ_GRID_POWER_ID].key = "domoticz_gridpower_idx";
    Lista[CONFIG_KEY_DOMOTICZ_CONSUMTION_POWER_ID].key = "domoticz_consumption_idx";
    Lista[CONFIG_KEY_DOMOTICZ_PV1_ID].key = "domoticz_pv1_string_idx";
    Lista[CONFIG_KEY_DOMOTICZ_PV1_ID].key = "domoticz_pv2_string_idx";
    
    
    ConfigClearValue(&Lista, CONFIG_KEY_END_LIST);
    
    cerr = ConfigFileRead(CONFFILE, Lista, CONFIG_KEY_END_LIST);
    if (cerr)
    {
        syslog(LOG_ERR, "InverterBroker: unable to open config file");
        if(print_debug_info) puts("InverterBroker: unable to open config file");
        return EXIT_FAILURE;
    }
 
    // Logger parameters 
    config_SetLoggerPar(Lista[CONFIG_KEY_LOGGER_SN].value,
                        Lista[CONFIG_KEY_LOGGER_IPADD].value,
                        Lista[CONFIG_KEY_LOGGER_PORT].value,
                        Lista[CONFIG_KEY_REFRESH_TIME].value);

    config_SetMqttPar(Lista[CONFIG_KEY_MQTT_SERVER].value,
                      Lista[CONFIG_KEY_MQTT_CLIENTID].value,
                      Lista[CONFIG_KEY_MQTT_TOPIC].value,
                      Lista[CONFIG_KEY_MQTT_BINARY_TOPIC].value);
    
    config_SetDomoticzPar(Lista[CONFIG_KEY_DOMOTICZ_TOPIC].value,
                          Lista[CONFIG_KEY_DOMOTICZ_PV_POWER_ID].value,
                          Lista[CONFIG_KEY_DOMOTICZ_PV_POWER_ID].value,
                          Lista[CONFIG_KEY_DOMOTICZ_GRID_POWER_ID].value,
                          Lista[CONFIG_KEY_DOMOTICZ_CONSUMTION_POWER_ID].value,
                          Lista[CONFIG_KEY_DOMOTICZ_PV1_ID].value,
                          Lista[CONFIG_KEY_DOMOTICZ_PV2_ID].value);
    
    /* Service initialize
     * port, refresh time
     */
    cerr = config_SetServicePar(Lista[CONFIG_KEY_SERVICE_PORT].value,
                                Lista[CONFIG_KEY_START_PARAM].value );
    
    
    return cerr;
}
