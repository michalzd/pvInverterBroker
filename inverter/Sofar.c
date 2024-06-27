/*
 * LogerSofar.c
 *
 *  Created on: May 4, 2023
 *      Author: mimi
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <resolv.h>
#include <unistd.h>
#include <string.h>

#include "Sofar.h"
#include "Config.h"
#include "Logger.h"


#define REQUEST_DBG 0
#define RESPONSE_DBG 0

struct SofarLogger SofarLogger;


struct  LoggerRequestFrame
{
	uint8_t  start; 		// Logger Start code
	uint8_t  framelen[2];		// Logger frame DataLength
	uint8_t  codebegin[2];	        // Logger Control Code 'Begin'
	uint8_t  sn[6];
	uint8_t  data[23];		// modbus full frame
	uint8_t  cs;
	uint8_t  codeend;
};


struct  LoggerResponseFrame
{
	uint8_t  start; 		// Logger Start code
	uint8_t  framelen[2];		// Logger frame DataLength
	uint8_t  codebegin[2];	        // Logger Control Code 'Begin'
	uint8_t  sn[6];
	uint8_t  data[256];		// modbus full frame
	uint8_t  cs;
	uint8_t  codeend;
};

union  SerialNo
{
	uint64_t  sn;
	uint8_t   byte[8];
};

static int  loggerSck;
static int  sck_error;
static int  lastState;
static int  lastPower = 0;
static int  lastAvrPower = 0;

static
uint64_t sofar_GetSerialNo(void)
{
	uint64_t tsn = SofarLogger.sn;
	union  SerialNo sn;
	sn.byte[0] = tsn;
	tsn = tsn >> 8;
	sn.byte[1] = tsn;
	tsn = tsn >> 8;
	sn.byte[2] = tsn;
	tsn = tsn >> 8;
	sn.byte[3] = tsn;
	tsn = tsn >> 8;
	sn.byte[4] = tsn;
	tsn = tsn >> 8;
	sn.byte[5] = tsn;
	tsn = tsn >> 8;
	sn.byte[6] = tsn;
	tsn = tsn >> 8;
	sn.byte[7] = tsn;
	return sn.sn;
}




static
uint8_t getFrameCheckSum( uint8_t *framedata, uint8_t size)
{
	register uint8_t checksum = 0;
	register uint8_t d;
	int i;

	if(REQUEST_DBG) printf(" cs d:");

	framedata++;  //checksum liczone bez znaku start
	size--;
	for(i = 0; i< size; i++) {
		d = *(framedata + i);
		if(REQUEST_DBG) printf( "%#x ", d);
		checksum += d;
	}
	return checksum;
}

static
uint8_t getResponseCheckSum( uint8_t *framedata, uint16_t recvsize)
{
	return( *(framedata+recvsize-2));
}

static
uint8_t getResponseCodeEnd( uint8_t *framedata, uint16_t recvsize)
{
	return( *(framedata+recvsize-1));
}


int logger_sofar_connect()
{
	struct sockaddr_in serverAddr;

	if(loggerSck) close(loggerSck);
	loggerSck = socket(AF_INET, SOCK_STREAM, 0);
	if(loggerSck<0) {
		sck_error = errno;
		loggerSck=0;
		return IME_RETURN_ERR_SOCK;
	}

	struct timeval tv;
	tv.tv_sec = 30; // receive timeout_in_seconds;
	tv.tv_usec = 0;
	setsockopt(loggerSck, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof tv);

	serverAddr.sin_family = AF_INET;
	serverAddr.sin_addr.s_addr = SofarLogger.address;
	serverAddr.sin_port = htons(SofarLogger.port);
        
	int err = connect(loggerSck, (struct sockaddr *) &serverAddr, sizeof(serverAddr) );
	if(err<0) {
		sck_error = errno;
		close(loggerSck);
		loggerSck=0;
		return IME_RETURN_ERR_SOCK_CONN;
	}

	return IME_RETURN_CODE_OK;
}


void logger_sofar_diconnect()
{
	if(loggerSck) close(loggerSck);
	loggerSck = 0;
}



int logger_sofar_SendRequest(const ModBus_Request_t * modbusrequest)
{
	int slen;
	struct  LoggerRequestFrame requestframe;

	// Logger Header
	// A5 1700 1045
	requestframe.start = 0xA5;
	requestframe.framelen[0] = 0x17;
	requestframe.framelen[1] = 00;
	requestframe.codebegin[0] = 0x10;
	requestframe.codebegin[1] = 0x45;

	// serial no
	union  SerialNo sn;
        sn.sn = sofar_GetSerialNo();
	requestframe.sn[0] = 0x00;
	requestframe.sn[1] = 0x00;
	requestframe.sn[2] = sn.byte[0];
	requestframe.sn[3] = sn.byte[1];
	requestframe.sn[4] = sn.byte[2];
	requestframe.sn[5] = sn.byte[3];

	// modbus ADU data start:
	// 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00
	requestframe.data[ 0] = 0x02;
	requestframe.data[ 1] = 0x00;
	requestframe.data[ 2] = 0x00;
	requestframe.data[ 3] = 0x00;
	requestframe.data[ 4] = 0x00;
	requestframe.data[ 5] = 0x00;
	requestframe.data[ 6] = 0x00;
	requestframe.data[ 7] = 0x00;
	requestframe.data[ 8] = 0x00;
	requestframe.data[ 9] = 0x00;
	requestframe.data[10] = 0x00;
	requestframe.data[11] = 0x00;
	requestframe.data[12] = 0x00;
	requestframe.data[13] = 0x00;
	requestframe.data[14] = 0x00;

	requestframe.data[15] = modbusrequest->device;
	requestframe.data[16] = modbusrequest->functioncode;
	requestframe.data[17] = modbusrequest->firstreg >> 8;
	requestframe.data[18] = modbusrequest->firstreg;
	requestframe.data[19] = modbusrequest->quantity >> 8;
	requestframe.data[20] = modbusrequest->quantity;
	requestframe.data[21] = modbusrequest->crc;
	requestframe.data[22] = modbusrequest->crc >> 8;

	requestframe.cs = 0x00;
	requestframe.codeend = 0x00;

	requestframe.cs = getFrameCheckSum( (uint8_t*) &requestframe, sizeof(requestframe));
	requestframe.codeend = 0x15;

	//if(REQUEST_DBG){
	//	printf("\nCRC:%#x, CS:%#x\n", modbusrequest->crc, requestframe.cs);
	//}

	if(loggerSck<=0) return IME_RETURN_ERR_SOCK_SEND;

	slen = send(loggerSck, &requestframe, sizeof(struct LoggerRequestFrame), 0);
	if(slen<0) return IME_RETURN_ERR_SOCK_SEND;
	return IME_RETURN_CODE_OK;
}


int logger_sofar_RecvResponse(ModBus_Response_t * modbusresponse)
{
    int rsize;
    int16_t datasize;
    static struct LoggerResponseFrame responseframe;

    if(loggerSck<=0) return IME_RETURN_ERR_SOCK_RECV;

    rsize = recv(loggerSck , &responseframe , sizeof(responseframe) , 0);
    if(rsize<=0) return IME_RETURN_ERR_SOCK_RECV;

    responseframe.cs = getResponseCheckSum( (uint8_t *) &responseframe, rsize);
    responseframe.codeend = getResponseCodeEnd( (uint8_t *) &responseframe, rsize);

    datasize = (responseframe.framelen[1] << 8) + responseframe.framelen[0];

    // modbus full frame
    modbusresponse->device = responseframe.data[14];
    modbusresponse->functioncode = responseframe.data[15];
    modbusresponse->size = responseframe.data[16];
    modbusresponse->data = &responseframe.data[17];
    //  CRC on two bytes
    modbusresponse->crc = (responseframe.data[datasize-1] << 8) + responseframe.data[datasize-2];

    return IME_RETURN_CODE_OK;
}



int logger_sofar_refresh()
{
    int rv;
    logger_sofar_inverter_grid();       // stan sieci odczytuję zawsze
    rv = logger_sofar_inverter_state();
    logger_sofar_inverter_inputs(); 
    return rv;
}

/*
 * Sofar Modbus frames
 */
static
int Sofar_GetSysState(struct Sofar_SysStateInfo *sys_state_info)
{
    int rv;
    ModBus_Request_t modbusrequest;
    ModBus_Response_t modbusresponse;
    struct Sofar_SysStateInfo *responsedata;

    // Sofar Registers  0x0400 to 0x0416
    uint16_t registerFrom = 0x0400;
    uint16_t registerTo   = 0x0416;
    modbusrequest.device = 0x00;
    modbusrequest.functioncode = 0x03;
    modbusrequest.firstreg = registerFrom;
    modbusrequest.quantity = registerTo - registerFrom + 1; // 0x0080
    modbusrequest.crc = modbusrequest_crc(&modbusrequest, 6);

    rv = logger_sofar_SendRequest( &modbusrequest );
    if(rv!=IME_RETURN_CODE_OK) return rv; 						// send error if not zero
    rv = logger_sofar_RecvResponse(&modbusresponse );
    if(rv!=IME_RETURN_CODE_OK) return rv;

    responsedata = (struct Sofar_SysStateInfo *) modbusresponse.data;
    sys_state_info->SysState = ntohs(responsedata->SysState);
    
    // FaultTable[18]  
    for(int i=0; i<18; i++)
    {
        sys_state_info->FaultTable[i] = ntohs(responsedata->FaultTable[i]);
    }

    return IME_RETURN_CODE_OK;
}


static
int Sofar_GetOnGridPower(struct Sofar_OnGrigPowerOutput *grid_power_output)
{
    int rv;
    ModBus_Request_t modbusrequest;
    ModBus_Response_t modbusresponse;
    struct Sofar_OnGrigPowerOutput *responsedata;

    // Sofar Registers  0x0480 to 0x048a
    uint16_t registerFrom = 0x0480;
    uint16_t registerTo   = 0x048a;
    modbusrequest.device = 0x00;
    modbusrequest.functioncode = 0x03;
    modbusrequest.firstreg = registerFrom;
    modbusrequest.quantity = registerTo - registerFrom + 1; // 0x0080
    modbusrequest.crc = modbusrequest_crc(&modbusrequest, 6);

    rv = logger_sofar_SendRequest( &modbusrequest );
    if(rv!=IME_RETURN_CODE_OK) return rv; 						// send error if not zero
    rv = logger_sofar_RecvResponse(&modbusresponse );
    if(rv!=IME_RETURN_CODE_OK) return rv;
    
    responsedata = (struct Sofar_OnGrigPowerOutput *) modbusresponse.data;
    grid_power_output->FrequencyGrid = ntohs(responsedata->FrequencyGrid);
    grid_power_output->ActivePowerOutputTotal = ntohs(responsedata->ActivePowerOutputTotal);
    grid_power_output->ReactivePowerOutputTotal = ntohs(responsedata->ReactivePowerOutputTotal);         
    grid_power_output->ApparetPowerOutputTotal  = ntohs(responsedata->ApparetPowerOutputTotal);
    grid_power_output->ActivePowerPccTotal   = ntohs(responsedata->ActivePowerPccTotal);
    grid_power_output->ReactivePowerPccTotal = ntohs(responsedata->ReactivePowerPccTotal);
    grid_power_output->ApparetPowerPccTotal  = ntohs(responsedata->ApparetPowerPccTotal);
    
    return IME_RETURN_CODE_OK;
}


static
int Sofar_GetOnGridPhase(struct Sofar_OnGridPhasePower *grid_phase_power)
{
    int rv;
    ModBus_Request_t modbusrequest;
    ModBus_Response_t modbusresponse;
    struct Sofar_OnGridPhasePower *responsedata;

    // Sofar Registers  0x048d to 0x04bc
    uint16_t registerFrom = 0x048d;
    uint16_t registerTo   = 0x04bc;
    modbusrequest.device = 0x00;
    modbusrequest.functioncode = 0x03;
    modbusrequest.firstreg = registerFrom;
    modbusrequest.quantity = registerTo - registerFrom + 1; // 0x0080
    modbusrequest.crc = modbusrequest_crc(&modbusrequest, 6);

    rv = logger_sofar_SendRequest( &modbusrequest );
    if(rv!=IME_RETURN_CODE_OK) return rv; 						// send error if not zero
    rv = logger_sofar_RecvResponse(&modbusresponse );
    if(rv!=IME_RETURN_CODE_OK) return rv;
    
    responsedata = (struct Sofar_OnGridPhasePower *) modbusresponse.data;
    grid_phase_power->Voltage_R = ntohs(responsedata->Voltage_R);
    grid_phase_power->Current_R = ntohs(responsedata->Current_R);
    grid_phase_power->ActivePower_R = ntohs(responsedata->ActivePower_R);
    grid_phase_power->ReactivePower_R = ntohs(responsedata->ReactivePower_R);
    grid_phase_power->PowerFactor_R = ntohs(responsedata->PowerFactor_R);
    
    grid_phase_power->Voltage_S = ntohs(responsedata->Voltage_S);
    grid_phase_power->Current_S = ntohs(responsedata->Current_S);
    grid_phase_power->ActivePower_S = ntohs(responsedata->ActivePower_S);
    grid_phase_power->ReactivePower_S = ntohs(responsedata->ReactivePower_S);
    grid_phase_power->PowerFactor_S = ntohs(responsedata->PowerFactor_S);
    
    grid_phase_power->Voltage_T = ntohs(responsedata->Voltage_T);
    grid_phase_power->Current_T = ntohs(responsedata->Current_T);
    grid_phase_power->ActivePower_T = ntohs(responsedata->ActivePower_T);
    grid_phase_power->ReactivePower_T = ntohs(responsedata->ReactivePower_T);
    grid_phase_power->PowerFactor_T = ntohs(responsedata->PowerFactor_T);
     
    return IME_RETURN_CODE_OK;
}

int Sofar_GetPVInput(struct Sofar_PVInput *pv_input)
{
    int rv;
    ModBus_Request_t modbusrequest;
    ModBus_Response_t modbusresponse;
    struct Sofar_PVInput *responsedata;

    // Sofar Registers  0x048d to 0x04bc
    uint16_t registerFrom = 0x0580;
    uint16_t registerTo   = 0x058c;
    modbusrequest.device = 0x00;
    modbusrequest.functioncode = 0x03;
    modbusrequest.firstreg = registerFrom;
    modbusrequest.quantity = registerTo - registerFrom + 1; // 0x0080
    modbusrequest.crc = modbusrequest_crc(&modbusrequest, 6);

    rv = logger_sofar_SendRequest( &modbusrequest );
    if(rv!=IME_RETURN_CODE_OK) return rv;
    rv = logger_sofar_RecvResponse(&modbusresponse );
    if(rv!=IME_RETURN_CODE_OK) return rv;
    
    responsedata = (struct Sofar_PVInput *) modbusresponse.data;
    pv_input->VoltagePV1 = ntohs(responsedata->VoltagePV1);
    pv_input->CurrentPV1 = ntohs(responsedata->CurrentPV1);
    pv_input->PowerPV1 = ntohs(responsedata->PowerPV1);
    pv_input->VoltagePV2 = ntohs(responsedata->VoltagePV2);
    pv_input->CurrentPV2 = ntohs(responsedata->CurrentPV2);
    pv_input->PowerPV2   = ntohs(responsedata->PowerPV2);
    pv_input->VoltagePV3 = ntohs(responsedata->VoltagePV3);
    pv_input->CurrentPV3 = ntohs(responsedata->CurrentPV3);
    pv_input->PowerPV3   = ntohs(responsedata->PowerPV3);
     
    return IME_RETURN_CODE_OK;
}


/*
 * convert sofar state into enum
 * SOFAR operating status
		0: waiting state
		1: Detection status
		2: Grid-connected status
		3: Emergency power supply status
		4: Recoverable fault state
		5: Permanent fault status
		6: Upgrade status
		7: Self-charging status
 */

static
uint8_t Sofar_StateConvert( uint16_t sofar_state)
{
    if(sofar_state==0) return InverterStateWait;
    if(sofar_state==1) return InverterStateGridDetect;
    if(sofar_state==2) return InverterStateNormal;
    if(sofar_state==4) return InverterStateGridFault;
    return 	InverterStateErr;
}



/* 
 * Logger functions
 */

int logger_sofar_inverter_state()
{
    struct Sofar_SysStateInfo 	sysState;
    struct Sofar_OnGrigPowerOutput	powerOutput;
    int rcv;
    
    inverterState.state = lastState;
    inverterState.activepower = lastPower;

    rcv = Sofar_GetSysState(&sysState);
    if(rcv) return rcv;
    
    inverterState.state = Sofar_StateConvert(sysState.SysState);
    if(sysState.SysState==2)  
    {
        Sofar_GetOnGridPower(&powerOutput);
        inverterState.activepower = powerOutput.ActivePowerOutputTotal;
        if(inverterState.activepower==0) inverterState.activepower = lastPower;
    }
    else 
    {
        // czasem bywa dziwny status, ale produkcja idzie, przestawiam to na stan normalny
        if(sysState.SysState > 2)
           if(gridState.Rcurrent > 100 || gridState.Scurrent > 100 || gridState.Tcurrent > 100) inverterState.state = 0; 
    
        if(inverterState.state==InverterStateGridFault)  inverterState.activepower = 0;
    }
    
    lastState = inverterState.state;
    lastPower = inverterState.activepower;
      
    return IME_RETURN_CODE_OK;
}


int logger_sofar_inverter_inputs()
{
    int rcv;
    struct Sofar_PVInput pv;

    rcv = Sofar_GetPVInput(&pv);
    if(rcv) return rcv;

    InverterInputPV1.voltage = pv.VoltagePV1;
    InverterInputPV1.current = pv.CurrentPV1;
    InverterInputPV2.voltage = pv.VoltagePV2;
    InverterInputPV2.current = pv.CurrentPV2;
    return IME_RETURN_CODE_OK;
}


int logger_sofar_inverter_grid()
{
    struct Sofar_OnGridPhasePower PhaseState;
    int rcv;

    rcv = Sofar_GetOnGridPhase(&PhaseState);
    if(rcv) return rcv;

    if(PhaseState.Voltage_R>2800 || PhaseState.Voltage_S>2800 || PhaseState.Voltage_T>2800){
            rcv = Sofar_GetOnGridPhase(&PhaseState);
            if(rcv) return rcv;
    }
    if(PhaseState.Voltage_R>2800 || PhaseState.Voltage_S>2800 || PhaseState.Voltage_T>2800) return IME_RETURN_ERR_SOCK_RECV;

    gridState.Rvoltage = PhaseState.Voltage_R;
    gridState.Rcurrent = PhaseState.Current_R;
    gridState.Svoltage = PhaseState.Voltage_S;
    gridState.Scurrent = PhaseState.Current_S;
    gridState.Tvoltage = PhaseState.Voltage_T;
    gridState.Tcurrent = PhaseState.Current_T;

    return IME_RETURN_CODE_OK;
}

