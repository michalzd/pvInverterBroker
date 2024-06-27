/*
 ============================================================================
 Name        : ImeService.c
 Author      : mimi
 Version     : 02.2023
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : wątek servisu  
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <errno.h>
#include <arpa/inet.h>
#include <BrokerService.h> 
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <resolv.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <mqtt/MQTTClient.h>

#include "Ime.h"
#include "Service.h" 
#include "MqttService.h"
#include "Config.h"  

#define SUBTOPIC_SZ 40

struct Subtopic 
{
    char json[SUBTOPIC_SZ];
    char state[SUBTOPIC_SZ];  
};

#define  SRV_BUFFER_SIZE 128
static uint8_t	srvbufrd[SRV_BUFFER_SIZE];
static uint8_t	srvbufwr[SRV_BUFFER_SIZE];

static Network mqttnet;
static MQTTClient mqttclient;
static struct Subtopic mqtt_subtopic;

extern struct InverterInfo inverterInfo;

/*
 * initialize 
 */
void mqtt_service_init()
{
    NetworkInit(&mqttnet);
    MQTTClientInit(&mqttclient, &mqttnet, 5000, srvbufwr, SRV_BUFFER_SIZE, srvbufrd, SRV_BUFFER_SIZE);
    
    if(config.mqtt.topic[0] == 0) 
    {
        config.mqtt.port = 0; 
        return;
    }
    
    size_t topiclen = strlen(config.mqtt.topic);
     
    strncpy(mqtt_subtopic.json, config.mqtt.topic, SUBTOPIC_SZ);
    strcpy(mqtt_subtopic.json + topiclen, "/s/json");
    
    strncpy(mqtt_subtopic.state, config.mqtt.topic, SUBTOPIC_SZ);
    strcpy(mqtt_subtopic.json + topiclen, "/s/state");
     
}

/*
 * connect to mqtt server
 */
static inline
int mqtt_connect_to_server()
{
    int rc = NetworkConnect(&mqttnet, config.mqtt.host, config.mqtt.port);
    if(rc) 
    {
        syslog (LOG_ERR, "InverterBroker: MQTT broker connect to %s error: %i", config.mqtt.host, errno );
        if(print_debug_info)
        {
            printf("InverterBroker MQTT broker connect to %s error: %i", config.mqtt.host, errno );
            puts("");
        }
    }
    return rc;
}

/*
 * send connect to mqtt server
 */
static
int mqtt_connect_send()
{
    MQTTPacket_connectData data = MQTTPacket_connectData_initializer;       
    data.willFlag = 0;
    data.MQTTVersion = 3;
    data.clientID.cstring = config.mqtt.clientid;
    data.username.cstring = NULL;
    data.password.cstring = NULL;
    if(*config.mqtt.user) data.username.cstring = config.mqtt.user;
    if(*config.mqtt.passwd) data.password.cstring = config.mqtt.passwd;
    data.keepAliveInterval = 60;
    data.cleansession = 1;
    
    int rc = MQTTConnect(&mqttclient, &data);
    if(rc)
    {
        syslog (LOG_INFO, "InverterBroker: MQTT connected error"); 
        if(print_debug_info)
        {
            printf("InverterBroker: MQTT connected error %i", rc);
            puts("");
        }
    }
    
    return rc;
}

 
int  mqtt_service_connect()
{
    register int rc = 0;
    
    if(config.mqtt.port == 0) 
    {
        syslog (LOG_ERR, "InverterBroker: MQTT not defined" );
        return IME_RETURN_ERR_INADDR;
    }
     
    rc = mqtt_connect_to_server();
    if(rc)  return IME_RETURN_ERR_SOCK;
    
    rc = mqtt_connect_send();
    if(rc)  return IME_RETURN_ERR_SOCK_SEND;
   
    syslog (LOG_INFO, "InverterBroker: MQTT connected, client Id: %s", config.mqtt.clientid); 
    if(print_debug_info)
    {
        printf("InverterBroker: MQTT connected, client Id: %s", config.mqtt.clientid);
        puts("");
    }
     
    return IME_RETURN_CODE_OK;
}


void mqtt_service_disconnect()
{
    MQTTDisconnect(&mqttclient);
    NetworkDisconnect(&mqttnet);
}


void mqtt_service_keep_alive()
{
    if(MQTTIsConnected(&mqttclient)) 
    {
        MQTTKeepalive(&mqttclient);
        return;
    }
    mqtt_service_disconnect();
    mqtt_service_connect();    
}


/* 
 * publish mqtt message as json object
 */
static
int  mqtt_service_publish_json()
{
    #define MSGJSONMAXSIZE 64
    char json[MSGJSONMAXSIZE];
    
    size_t len = snprintf(json, MSGJSONMAXSIZE, "{Time:[%.2i,%.2i,%.2i,%i],State:[%i,%i0,%i0],Grid[%i,%i,%i]}",
                inverterInfo.InverterState.tmhour, inverterInfo.InverterState.tmmin, inverterInfo.InverterState.tmsec, inverterInfo.InverterState.tmweekday,
                inverterInfo.InverterState.state, ntohs(inverterInfo.InverterState.activepower), ntohs(inverterInfo.InverterState.averagepower), 
                ntohs(inverterInfo.Grid.Rvoltage), ntohs(inverterInfo.Grid.Svoltage), ntohs(inverterInfo.Grid.Tvoltage) );
    
    MQTTMessage mqttmsg;
    mqttmsg.qos = QOS1;
    mqttmsg.dup = 0; 
    mqttmsg.payload = json;
    mqttmsg.payloadlen = ++len;
    
    int rc = MQTTPublish(&mqttclient, mqtt_subtopic.json, &mqttmsg); 
    if(rc)
    {
        syslog (LOG_INFO, "InverterBroker: MQTT publish json error %i", rc);
        if(print_debug_info)
        {
            printf("InverterBroker: MQTT publish json %i", rc);
            puts("");
        }
    }
    return rc;
}


static
int  mqtt_service_publish_state()
{
    #define MSGSTATEMAXSIZE 32
    char  msgstate[MSGSTATEMAXSIZE];
     
    size_t len = snprintf(msgstate, MSGSTATEMAXSIZE, "S%i, P %i0, Avg %i0", 
                          inverterInfo.InverterState.state,
                          ntohs(inverterInfo.InverterState.activepower), 
                          ntohs(inverterInfo.InverterState.averagepower) );
    
    MQTTMessage mqttmsg;
    mqttmsg.qos = QOS1;
    mqttmsg.dup = 0; 
    mqttmsg.payload = msgstate;
    mqttmsg.payloadlen = len;
    
    int rc = MQTTPublish(&mqttclient, mqtt_subtopic.state, &mqttmsg); 
    if(rc)
    {
        syslog (LOG_INFO, "InverterBroker: MQTT publish state error %i", rc);
        if(print_debug_info)
        {
            printf("InverterBroker: MQTT publish state %i", rc);
            puts("");
        }
    }
    return rc;
}


/* 
 * publish mqtt message as struct data object
 */
static
int  mqtt_service_publish_data()
{ 
    MQTTMessage mqttmsg;
    mqttmsg.qos = QOS1;
    mqttmsg.dup = 0; 
    mqttmsg.payload = &(inverterInfo.InverterState);
    mqttmsg.payloadlen = sizeof(struct InverterInfo);
    
    int rc = MQTTPublish(&mqttclient, config.mqtt.binarytopic, &mqttmsg); 
    if(rc)
    {
        syslog (LOG_INFO, "InverterBroker: MQTT publish binary data error %i", rc);
        if(print_debug_info)
        {
            printf("InverterBroker: MQTT publish binary data %i", rc);
            puts("");
        }
    }
    
    return rc;
}

/* 
 * publish mqtt message from inverter  
 */
int  mqtt_service_publish()
{
    register int rc = 0;
    
    rc = mqtt_service_publish_data();
    if(rc) return rc;
    
    rc = mqtt_service_publish_state();
    if(rc) return rc;
    
    rc = mqtt_service_publish_json();
    if(rc) return rc;
    
    return rc;
}
