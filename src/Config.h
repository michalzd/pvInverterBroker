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
#define CONFFILE "/etc/ime/inverterbroker"


#ifdef PARAM_OPENWRT
#define VAR_LOG_FILE "/var/inverterbroker.log"
#else
#define VAR_LOG_FILE "/var/tmp/inverterbroker.log"
#endif


struct	UdpService 
{
    uint16_t    port;
    const char *logfilename;
};

#define MQTT_CLIENTID_SZ   8
#define MQTT_TOPIC_SZ   32
#define MQTT_USERNAME_PASS_SZ   1
#define MQTT_HOST_SZ  16

struct	MqttServer
{
    int    port; 
    char   clientid[MQTT_CLIENTID_SZ];
    char   host[MQTT_HOST_SZ];
    char   user[MQTT_USERNAME_PASS_SZ];
    char   passwd[MQTT_USERNAME_PASS_SZ];
    char   topic[MQTT_TOPIC_SZ];
    char   binarytopic[MQTT_TOPIC_SZ];
};


struct ConfigBroker
{
    struct  UdpService  service;
    struct  MqttServer  mqtt;
};


/*
 * lista kluczy
 */
enum CONFIG_KEY
{
    // inverter Modbus logger 
    CONFIG_KEY_LOGGER_SN = 0,
    CONFIG_KEY_LOGGER_IPADD,
    CONFIG_KEY_LOGGER_PORT,
    CONFIG_KEY_REFRESH_TIME,
    // jeśli podane, uruchomiony będzie servis UDP
    CONFIG_KEY_SERVICE_PORT,
    CONFIG_KEY_START_PARAM, 
    // adres MQTT servera, jeśli port servera jest inny niż domyślny, podać po ':'
    CONFIG_KEY_MQTT_SERVER, 
    CONFIG_KEY_MQTT_CLIENTID, 
    CONFIG_KEY_MQTT_TOPIC,
    CONFIG_KEY_MQTT_BINARY_TOPIC,
    
    CONFIG_KEY_END_LIST
};

extern struct	ConfigBroker	config;

int config_ReadFile(void);
int config_SetServicePar( const char *ip_port, const char *startup );

#endif /* IMECONFIG_H_ */
