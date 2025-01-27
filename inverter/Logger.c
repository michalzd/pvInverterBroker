/*
 ============================================================================
 Name        : Logger.H
 Author      : mimi
 Version     : 01.2003
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : komunikacja z loggerem falownika 
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <time.h> 
#include "Logger.h" 

#include "Sofar.h"


/* 
 * do obliczenia średniej napięcia fazowego 
 */
#define VOLTAGES_ARRAY_SIZE 10
static uint16_t Rvoltages[VOLTAGES_ARRAY_SIZE];
static uint16_t Svoltages[VOLTAGES_ARRAY_SIZE];
static uint16_t Tvoltages[VOLTAGES_ARRAY_SIZE];



/* 
 * funkcje komunikacji z falownikiem
 * podstawiamy właściwe dla invertera
 */

static 
int  inverter_connect()
{
    return logger_sofar_connect();
}

static inline 
void inverter_diconnect()
{
    logger_sofar_diconnect();
}

static inline 
int  inverter_data_read()
{
    return logger_sofar_refresh();
}

/* Logger parameters
 * sn connection addres and port,
 */
void logger_set_parameters( const char *serialno, const char *ip_address, int ip_port )
{
    char *end;
    SofarLogger.sn = strtoll(serialno, &end, 10);
    inet_pton(AF_INET, ip_address, &SofarLogger.address);
    SofarLogger.port = ip_port; 
}


uint8_t logger_get_min_tens(uint8_t minute)
{
    if(minute<10) return 0;
    if(minute<20) return 10;
    if(minute<30) return 20;
    if(minute<40) return 30;
    if(minute<50) return 40;
    return 50;
}



static 
void logger_clear_voltages()
{
    int i;
    register int v=2300;
    for(i=0; i<VOLTAGES_ARRAY_SIZE; i++)
    {
        Rvoltages[i] = v;
        Svoltages[i] = v;
        Tvoltages[i] = v;
    }
}

static
uint16_t logger_get_average_voltage(uint8_t  minute, uint16_t voltages[], uint16_t current_voltage)
{
    uint32_t suma = 0; 
    uint8_t i = (minute - logger_get_min_tens(minute) );
    voltages[i] = current_voltage;
    for(i=0; i<VOLTAGES_ARRAY_SIZE; i++) suma+= voltages[i];
    return (suma / 10); 
}


/*
 *  
 */

void logger_refresh_time_info()
{ 
    time_t timer = time(NULL);
    struct tm* time_info = localtime(&timer);
    inverterState.tmsec = time_info->tm_sec;
    inverterState.tmmin = time_info->tm_min;
    inverterState.tmhour = time_info->tm_hour;
    inverterState.tmweekday = time_info->tm_wday;
}


void logger_refresh_state()
{
    /*
     * okreslam najwyższe napięcie na fazach
     */
    gridState.voltage = gridState.Rvoltage;
    gridState.maxphase = Phase_R;
    if(gridState.voltage<gridState.Svoltage)
    {
            gridState.voltage = gridState.Svoltage;
            gridState.maxphase = Phase_S;
    }
    if(gridState.voltage<gridState.Tvoltage)
    {
            gridState.voltage = gridState.Tvoltage;
            gridState.maxphase = Phase_T;
    }
    
    /* 
     * która faza przekroczona
     */
    gridState.phaseOverV = 0;
    if(gridState.Rvoltage > OVERVOLTAGE_LIMIT) gridState.phaseOverV |= Phase_R;
    if(gridState.Svoltage > OVERVOLTAGE_LIMIT) gridState.phaseOverV |= Phase_S;
    if(gridState.Tvoltage > OVERVOLTAGE_LIMIT) gridState.phaseOverV |= Phase_T;

}


void logger_refresh_average_power()
{
    static uint32_t  powersum = 0;
    static int       lastminsum = 0;
    static uint32_t  lastAvrPower = 0;
    int16_t activepwr;
    int deltaminunts;
    
    inverterState.averagepower = lastAvrPower;
    activepwr = inverterState.activepower;
    if(activepwr < 0) activepwr = 0;
    
    if(inverterState.tmmin < lastminsum)
    {
        // next hour, clear  
        inverterState.averagepower = activepwr;
        powersum = 0;
        lastminsum = 0; 
    }
    
    if(inverterState.tmmin != lastminsum) 
    {
        deltaminunts = inverterState.tmmin - lastminsum;
        powersum += (activepwr * deltaminunts);
        deltaminunts = inverterState.tmmin;
        if(deltaminunts==0) deltaminunts = 1;
        inverterState.averagepower = powersum / deltaminunts;
    }

    lastAvrPower = inverterState.averagepower;
    lastminsum = inverterState.tmmin;
}
    

int logger_refresh()
{
    static uint iconnerr=0;
    uint8_t	 error_code = 0;
    
    logger_refresh_time_info();
    
    int rv = inverter_connect();
    if(rv) error_code = InverterStateConnectionErr;
    else
    {
        iconnerr = 0;
        rv = inverter_data_read();
        if(rv) error_code = InverterStateSocketErr;
        logger_refresh_state();
    }
    inverter_diconnect();
    
    if(error_code)
    {
        iconnerr++;
        if(iconnerr > 3) logger_clear_data();
        inverterState.state = InverterStateConnectionErr;
        inverterState.activepower = 0;
        rv = error_code;
    }
    
    /* srednia moc z bieżącej godziny 
     * i srednie napięcia z ostatnich 10 minut
     */
    logger_refresh_average_power();
    gridState.Ravgvoltage = logger_get_average_voltage(inverterState.tmmin, Rvoltages, gridState.Rvoltage);
    gridState.Savgvoltage = logger_get_average_voltage(inverterState.tmmin, Svoltages, gridState.Svoltage);
    gridState.Tavgvoltage = logger_get_average_voltage(inverterState.tmmin, Tvoltages, gridState.Tvoltage);
    
    return rv;
}


void logger_clear_data()
{
	inverterState.activepower = 0;
        inverterState.averagepower = 0;
	gridState.Rvoltage = 0;
	gridState.Rcurrent = 0;
	gridState.Svoltage = 0;
	gridState.Scurrent = 0;
	gridState.Tvoltage = 0;
	gridState.Tcurrent = 0;

	InverterInputPV1.voltage = 0;
	InverterInputPV1.current = 0;

	InverterInputPV2.voltage = 0;
	InverterInputPV2.current = 0;

        logger_clear_voltages();
         
}
