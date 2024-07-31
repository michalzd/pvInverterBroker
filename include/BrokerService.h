/*
 ============================================================================
 Name        : BrokerService.h
 Author      : mimi
 Version     : 01.2023
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : typy i struktury przekazywane w komunikatach do/z brokera
 ============================================================================
 */

#ifndef BROKERSERVICE_H_
#define BROKERSERVICE_H_

#include <stdint.h>

/*
 * typ i struktury przekazywane w komunikatach UDP
 */
enum BrokerRequestType
{
    BrokerRequestTypeUndefined = 0,  
    BrokerRequestTypeInverterInfo,
    BrokerRequestTypeSetListener,
    BrokerRequestTypeInverterState,
    BrokerRequestTypeGridState,
    BrokerRequestTypeEnd
};




struct BrokerRequestInfo
{
    uint8_t RequestType; 
};

/*
 * rejestracja/ wyrejestrowanie listenera UDP
 * do zarejetrowanego listenera będzie automatycznie przekazywany komunikat struct InverterInfo
 * z aktualnym stanem   
 */
enum BrokerListener
{
    BrokerListenerUnregister = 0,
    BrokerListenerRegister 
};


struct BrokerRequestInfoListener
{
    uint8_t RequestType;
    uint8_t RegisterListener;
};


/*
 * to jest automatyczna odpowiedz na nieznany typ komunikatu request
 */
struct BrokerResponseHello
{
	uint8_t ResponseType;
	char    hello[6];
};  


enum InverterStateEnum
{
	InverterStateNormal = 0,
	InverterStateWait,
	InverterStateGridDetect,
	InverterStatePVErr,
	InverterStateGridFault,
	InverterStateErr,
	InverterStateInit,
	InverterStateOff = 0x10,
	InverterStateConnectionErr,
	InverterStateSocketErr
};


/*
 * struktira przekazywana w komunikacie UDP 
 * jako odpowiedz na BrokerRequestTypeInverterState
 */
struct  Inverter
{
	uint8_t   state;
	int16_t   activepower;
	int16_t	  averagepower;
	uint8_t   tmsec;		/* Second */
	uint8_t   tmmin;		/* Minutes.	[0-59] */
	uint8_t   tmhour;		/* Hours.	[0-23] */
	uint8_t   tmweekday;		/* Day of week.	[0-6] */
};


struct  InverterPV
{
	uint16_t voltage;
	uint16_t current;
};


enum PhaseNo
{
	Phase_R = 0x10,	// faza R
	Phase_S = 0x20,	// faza S
	Phase_T = 0x40,	// faza T
};



/*
 * struktira przekazywana w komunikacie UDP 
 * jako odpowiedz na BrokerRequestTypeGridState
 */
struct Grid
{
    uint16_t Rvoltage;		//  0.1 V
    uint16_t Rcurrent;		// 0.01 A
    uint16_t Svoltage;
    uint16_t Scurrent;
    uint16_t Tvoltage;
    uint16_t Tcurrent;
    uint16_t voltage;
    uint8_t  maxphase;
    uint8_t  phaseOverV;
    uint16_t Ravgvoltage;       // grid R average voltage
    uint16_t Savgvoltage;       // grid S average voltage
    uint16_t Tavgvoltage;       // grid T average voltage   
};


/*
 * struktira przekazywana w komunikacie UDP 
 * jako odpowiedz na BrokerRequestTypeInverterInfo
 * 
 * jest również przekazywana automatycznie do zarejestrowanego Listenera
 */
struct InverterInfo
{
    struct  Inverter 	InverterState;
    struct  InverterPV	PV1;
    struct  InverterPV	PV2;
    struct  Grid	Grid;
};


#endif /* BROKERSERVICE_H_ */
