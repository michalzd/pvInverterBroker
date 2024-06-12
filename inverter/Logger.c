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

void logger_refresh_average()
{
    static uint32_t  powersum = 0;
    static uint8_t   lastminsum = 0;
    static uint32_t  lastAvrPower = 0;
    int dminunts;
    
    inverterState.averagepower = lastAvrPower;
    
    if(inverterState.tmmin == 1 && lastminsum==0) 
    {
        inverterState.averagepower = 0 ;
        powersum = 0;
    }
    if(inverterState.tmmin != lastminsum) {
        powersum += inverterState.activepower;
        dminunts = inverterState.tmmin;
        if(dminunts==0) dminunts = 60;
        inverterState.averagepower = powersum / dminunts;
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
    
    logger_refresh_average();

    return rv;
}


int logger_clear_data()
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

	return IME_RETURN_CODE_OK;
}
