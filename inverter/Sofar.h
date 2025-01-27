/*
 ============================================================================
 Name        : LoggerSofar.h
 Author      : mimi
 Version     : 01.2003
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : komunikacja z loggerem falownika Sofar KTLX-G3 
 ============================================================================
 */

#ifndef SOFARCLIENT_H_
#define SOFARCLIENT_H_

#include <stdlib.h>
#include <stdint.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include "ModbusAdu.h"

/* server Sofar Logger LSW-3 by TCP */
struct	SofarLogger
{
    in_addr_t   address;
    uint16_t    port;
    uint64_t    sn;
};

extern struct SofarLogger SofarLogger;

int  logger_sofar_connect();
void logger_sofar_diconnect();
int  logger_sofar_refresh();

/*
 *  send/receive modbus data
 */
int logger_sofar_SendRequest(const ModBus_Request_t * modbusrequest);
int logger_sofar_RecvResponse(ModBus_Response_t * modbusrequest);

int logger_sofar_inverter_state();
int logger_sofar_inverter_error( uint8_t errmsg);
int logger_sofar_inverter_inputs();
int logger_sofar_inverter_grid();

/*
 * Sofar Modbus frames send/receive
 */

// Sofar Registers  0x0400 to 0x0416
struct Sofar_SysStateInfo
{
    uint16_t AddressMask[4];		// olewam
    uint16_t SysState;
    uint16_t FaultTable[18];
};


// Sofar Registers  0x0480 to 0x048a
struct Sofar_OnGrigPowerOutput
{
    uint16_t AddressMask[4];		// olewam
    uint16_t FrequencyGrid;             // 0x0484
    int16_t  ActivePowerOutputTotal;
    int16_t  ReactivePowerOutputTotal;
    int16_t  ApparentPowerOutputTotal;
    int16_t  ActivePowerPccTotal;       // 0x0488 positive to fed into the grid,negative to draw from the grid,
    int16_t  ReactivePowerPccTotal;    
    int16_t  ApparentPowerPccTotal;
};

// Sofar Registers  0x048d to 0x04bc
struct Sofar_OnGridPhasePower
{
    uint16_t Voltage_R;
    uint16_t Current_R;
    uint16_t ActivePower_R;
    uint16_t ReactivePower_R;
    uint16_t PowerFactor_R;
    uint16_t CurrentPcc_R;
    uint16_t ActivePowerPcc_R;
    uint16_t ReactivePowerPcc_R;
    uint16_t PowerFactorPcc_R;
    uint16_t Reserved1_R;
    uint16_t Reserved2_R;

    uint16_t Voltage_S;
    uint16_t Current_S;
    uint16_t ActivePower_S;
    uint16_t ReactivePower_S;
    uint16_t PowerFactor_S;
    uint16_t CurrentPcc_S;
    uint16_t ActivePowerPcc_S;
    uint16_t ReactivePowerPcc_S;
    uint16_t PowerFactorPcc_S;
    uint16_t Reserved1_S;
    uint16_t Reserved2_S;

    uint16_t Voltage_T;
    uint16_t Current_T;
    uint16_t ActivePower_T;
    uint16_t ReactivePower_T;
    uint16_t PowerFactor_T;
    uint16_t CurrentPcc_T;
    uint16_t ActivePowerPcc_T;
    uint16_t ReactivePowerPcc_T;
    uint16_t PowerFactorPcc_T;
    uint16_t Reserved1_T;
    uint16_t Reserved2_T;

    uint16_t ActivePowerPV;
    uint16_t ActivePowerLoad;

    uint16_t Voltage_L1N;
    uint16_t Current_L1N;
    uint16_t ActivePower_L1N;
    uint16_t CurrentPcc_L1N;
    uint16_t ActivePowerPcc_L1N;

    uint16_t Voltage_L2N;
    uint16_t Current_L2N;
    uint16_t ActivePower_L2N;
    uint16_t CurrentPcc_L2N;
    uint16_t ActivePowerPcc_L2N;

    uint16_t VoltageLine_L1;	// between R/S
    uint16_t VoltageLine_L2; 	// between S/T
    uint16_t VoltageLine_L3;	// between T/R
};


// Sofar Registers  0x0580 to 0x058c
struct Sofar_PVInput
{
    uint16_t AddressMask[4];		// olewam
    uint16_t VoltagePV1;
    uint16_t CurrentPV1;
    uint16_t PowerPV1;
    uint16_t VoltagePV2;
    uint16_t CurrentPV2;
    uint16_t PowerPV2;
    uint16_t VoltagePV3;
    uint16_t CurrentPV3;
    uint16_t PowerPV3;
    // (jest tego wiecej)
};

// Sofar Registers  0x0500  to 0x0507
struct Sofar_OffGridPower
{
    uint16_t AddressMask[4];		// olewam
    uint16_t ActivePowerLoadTotal;      //I16 0,01 kW R Load Active power Consumed by load is positive Feedback from load is negative End User 0 
    uint16_t ReactivePowerLoadTotal;    //I16 0,01 kW R Load Reactive power End User 0  
    uint16_t ApparentPowerLoadTotal;    //I16 0,01 kW R Load Apparent power Consumed by load is positive Feedback from load is negative End User 0 
    uint16_t Frequency;                 //U16 0,01 Hz R Frequency_Output
};


#endif /* SOFARCLIENT_H_ */
