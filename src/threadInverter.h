/*
 ============================================================================
 Name        : ImeInverter.h
 Author      : mimi
 Version     : 02.2023
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : komunikacja z inverterem PV
 ============================================================================
 */

#ifndef IMEINVERTERER_H_
#define IMEINVERTERER_H_

#include <stdint.h>
#include <sys/types.h>
#include "BrokerService.h"
#include "Ime.h"

enum SocketPair {
    parentsocket = 0,
    childsocket = 1
};

int     thread_inverter_init(); 
void    thread_inverter_set_refresh_time(uint16_t rtime);
int     thread_inverter_get_sck();
pid_t	thread_inverter_create();
int     thread_inverter_thread( void *none);
void    thread_inverter_end();

#endif /* IMEINVERTERER_H_ */
