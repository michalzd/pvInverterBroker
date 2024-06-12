/*
 ============================================================================
 Name        : Logger.H
 Author      : mimi
 Version     : 01.2003
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : komunikacja z loggerem falownika 
 ============================================================================
 */

#ifndef LOGGER___H_
#define LOGGER___H_

#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "BrokerService.h"
#include "Ime.h"

#define OVERVOLTAGE_LIMIT 2530


/*
 * struktury do wypełnienia przez funkcje komunikacji z falownikiem 
 */
extern volatile struct Inverter  inverterState;
extern volatile struct Grid	 gridState;

extern struct InverterPV InverterInputPV1;
extern struct InverterPV InverterInputPV2;

/* 
 * zapis parametrow odczytanych z pliku konfiguracyjnego 
 */
void logger_set_parameters( const char *serialno, const char *ip_address, int ip_port );

/*
 * zeruje struktury danych
 */
int logger_clear_data();

/*
 * odswiezanie danych 
 */
int logger_refresh();

#endif /* LOGGER___H_ */
