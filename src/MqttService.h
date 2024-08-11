/*
 ============================================================================
 Name        : MqttService.h
 Author      : mimi
 Version     : 01.2003
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : mqtt client 
 ============================================================================
 */

#ifndef MQTTSERVICE_H_
#define MQTTSERVICE_H_

#include "BrokerService.h"

void mqtt_service_init();
int  mqtt_service_connect();
void mqtt_service_disconnect();
void mqtt_service_keep_alive();


/* 
 * publish mqtt message  
 */
int  mqtt_service_publish();

#endif /* MQTTSERVICE_H_ */
