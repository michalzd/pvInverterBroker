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

int     ime_inverter_init(); 
void    ime_inverter_set_refresh_time(uint16_t rtime);
int     ime_inverter_thread_sck();
pid_t	ime_inverter_thread_create();
int     ime_inverter_thread( void *none);
void    ime_inverter_thread_end();

#endif /* IMEINVERTERER_H_ */
