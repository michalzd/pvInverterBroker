/*
 ============================================================================
 Name        : ImeService.c
 Author      : mimi
 Version     : 01.2003
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
#include "Ime.h"
#include "ImeInverter.h"
#include "ImeService.h"
#include "ImeConfig.h" 

#define  SRV_BUFFER_SIZE 255
static int   srvsocket;
static int   loggersocket;

static struct timeval timeout;  // Timeout for select
static volatile int   maxsckid;

static uint8_t	srvbuffer[SRV_BUFFER_SIZE];

static volatile int Listerer_sck = 0;
static struct sockaddr_in Listener;

static volatile int Controller_sck = 0;
static struct sockaddr_in Controller;


/* message from inverter logger 
 */
struct ServiceResponseInverterInfo inverterInfo;

static int ime_service_logger_msg(int sck);
static int ime_service_request(int sck); 
static int ime_service_send_InverterInfo();

/*
 * open UDP socket as server
 */
static
int	ime_service_socket_open(uint16_t sck_port )
{
    struct sockaddr_in sckaddr;
    socklen_t	rlen;
    int sck;
    int	err;

    rlen = sizeof(sckaddr);
    bzero(&sckaddr, rlen);

    sckaddr.sin_family = AF_INET;
    sckaddr.sin_port   = htons(sck_port);
    sckaddr.sin_addr.s_addr = INADDR_ANY;

    sck = socket(PF_INET, SOCK_DGRAM, 0);
    err = bind(sck, (struct sockaddr*) &sckaddr, rlen);
    if(err) {
        err = errno;
        close(sck);
        sck = 0;
    }
    return sck;
}


int  ime_service_start()
{
    register int s = 0;

    srvsocket = 0;
    s = ime_service_socket_open(service.port);
    if(s<=0) {
        syslog (LOG_ERR, "InverterBroker: socket error: %i", errno );
        if(print_debug_info)
        {
            printf("ImeBroker: port %i error ", service.port);
            puts("");
        }
        return IME_RETURN_ERR_SOCK;
    }
    syslog (LOG_INFO, "InverterBroker: socket port: %i", service.port ); 
    if(print_debug_info)
    {
        printf("ImeBroker: port %i ", service.port);
        puts("");
    }
    srvsocket = s;
    loggersocket = ime_inverter_thread_sck();
    
    return IME_RETURN_CODE_OK;
}


void ime_service_stop()
{
    if(srvsocket) close(srvsocket);
}

static
void  ime_service_fdset(fd_set *socketset, int fd)
{
    if(maxsckid < fd) maxsckid = fd;
    if(fd>0) 
    {
        FD_SET (fd,  socketset);
    }
}

int ime_service_thread()
{
    int 	v=0;
    int		sck;
    static  uint8_t	refreshtimer = 0;
    fd_set 	sckset;

    FD_ZERO(&sckset);
    FD_SET (srvsocket,  &sckset);
    ime_service_fdset(&sckset, srvsocket );
    ime_service_fdset(&sckset, loggersocket );  
    
   
    if(maxsckid)
    {
        // set timer
        timeout.tv_sec = 20;
        timeout.tv_usec = 0;

        v = select( maxsckid+1 , &sckset, (fd_set *)0, (fd_set *)0, &timeout);
        if (v == 0)
        {
            refreshtimer++; // timeout, można coś zrobić dodatkowego
        }
        if (v > 0)
        {
            sck = loggersocket;
            if (sck && FD_ISSET(sck, &sckset) ) ime_service_logger_msg(sck);
            
            sck = srvsocket;
            if (sck && FD_ISSET(sck, &sckset) ) ime_service_request(sck);

        }

    }

    return IME_RETURN_CODE_OK;
}

static 
int ime_service_logger_msg(int sck)
{
    static uint8_t lastmin=0;
    int n = recv(sck, &inverterInfo, sizeof(struct ServiceResponseInverterInfo), 0);
    
    if(print_debug_info)
    {
       printf(" PV State:%i Power: %i [dekaWat]  RST:%i %i %i [decyVolt]", 
              inverterInfo.InverterState.state, 
              ntohs(inverterInfo.InverterState.activepower),
              ntohs(inverterInfo.Grid.Rvoltage),
              ntohs(inverterInfo.Grid.Svoltage),
              ntohs(inverterInfo.Grid.Tvoltage)
              );
       puts("");
    }
    
    /* raz na minutę wysyłam info do zalogowanego nasłuchu
     */
    if(n && (lastmin != inverterInfo.InverterState.tmmin) ) 
    {
        ime_service_send_InverterInfo();
        lastmin = inverterInfo.InverterState.tmmin;
    }
    return n;
}


static
int ime_service_request_Listener(int sck, const struct sockaddr_in *si_sender, const socklen_t slen, void * request)
{
    struct ServiceRequestInverterInfoListener *listenerinfo = request;
    char buf[16];

    switch(listenerinfo->RegisterListener)
    {
      case InverterInfoListenerUnregister:
              bzero( (void*)&Listener, sizeof(Listener));
              Listerer_sck = 0;
              break;
      case InverterInfoListenerRegister:
              memcpy( (void*)&Listener, si_sender, sizeof(Listener) );
              Listerer_sck = sck;
              inet_ntop(AF_INET, &(Listener.sin_addr), buf, 20);
              syslog (LOG_INFO, "ImeBroker: Listener:%s", buf);
              break;
      case ControllerListenerUnregister:
              bzero( (void*)&Controller, sizeof(Controller));
              Controller_sck = 0;
              break;
      case ControllerListenerRegister:
              memcpy( (void*)&Controller, si_sender, sizeof(Controller) );
              Controller_sck = sck;
              inet_ntop(AF_INET, &(Controller.sin_addr), buf, 20);
              syslog (LOG_INFO, "ImeBroker: Controller:%s", buf);
              break;
    }

    sendto(sck, listenerinfo, sizeof(struct ServiceRequestInverterInfoListener), 0, (const struct sockaddr *) si_sender, slen);
    return IME_RETURN_CODE_OK;
}

static
int ime_service_request_InverterInfo(int sck, const struct sockaddr_in *si_sender, const socklen_t  slen)
{
    sendto(sck, &inverterInfo, sizeof(struct ServiceResponseInverterInfo), 0, (const struct sockaddr *) si_sender, slen);
    return IME_RETURN_CODE_OK;
}

static
int ime_service_request_InverterState(int sck, const struct sockaddr_in *si_sender, const socklen_t  slen)
{
    struct  Inverter *inverterState = &(inverterInfo.InverterState);
    sendto(sck, inverterState, sizeof(struct Inverter), 0, (const struct sockaddr *) si_sender, slen);
    return IME_RETURN_CODE_OK;
}

static 
int ime_service_request_GridState(int sck, const struct sockaddr_in *si_sender, const socklen_t  slen)
{
    struct  Grid *gridState = &(inverterInfo.Grid);
    sendto(sck, gridState, sizeof(struct Grid), 0, (const struct sockaddr *) si_sender, slen);
    return IME_RETURN_CODE_OK;
}

static
int ime_service_send_InverterInfo()
{
    if(Listerer_sck>0)   ime_service_request_InverterInfo(Listerer_sck, &Listener, sizeof(Listener) );
    if(Controller_sck>0) ime_service_request_InverterInfo(Controller_sck, &Controller, sizeof(Controller) );
    return IME_RETURN_CODE_OK;
}


static
int ime_service_request_InfoJson(int sck, const struct sockaddr_in *si_sender, const socklen_t  slen)
{
#define MSGINFOMAXSIZE 256
    char msginfo [MSGINFOMAXSIZE];  
    int  i, pos;
    
    pos = snprintf(msginfo, MSGINFOMAXSIZE, "{ Time:[%.2i,%.2i,%.2i,%i], Inverter:[%i,%i0,%i0,%i,%i,%i] } ",
		inverterInfo.InverterState.tmhour, inverterInfo.InverterState.tmmin, inverterInfo.InverterState.tmsec, inverterInfo.InverterState.tmweekday,
		inverterInfo.InverterState.state, ntohs(inverterInfo.InverterState.activepower), ntohs(inverterInfo.InverterState.averagepower), 
                ntohs(inverterInfo.Grid.Rvoltage), ntohs(inverterInfo.Grid.Svoltage), ntohs(inverterInfo.Grid.Tvoltage) );
   
    sendto(sck, msginfo, strlen(msginfo), 0, (const struct sockaddr *) si_sender, slen);
    return IME_RETURN_CODE_OK;
}


static
int ime_service_request_default(int sck, const struct sockaddr_in *si_sender, const socklen_t  slen)
{
    char *hello = "Hello";
    sendto(sck, (const char *)hello, strlen(hello),  0, (const struct sockaddr *) si_sender, slen);
    return IME_RETURN_CODE_OK;
}


/*
 * receive request
 */
static
int ime_service_request(int sck)
{
    int n;
    struct sockaddr_in si_sender;
    struct ServiceRequestInfo *request;

    socklen_t  slen = sizeof(si_sender);

    bzero(&si_sender, slen);

    n = recvfrom(sck, srvbuffer, SRV_BUFFER_SIZE, 0, (struct sockaddr *) &si_sender, &slen);

    if (n == -1) return 0;
    srvbuffer[SRV_BUFFER_SIZE-1] = 0;

    request = (struct ServiceRequestInfo *) srvbuffer;
    switch(request->RequestType)
    {
        case RequestTypeInverterInfo:
                ime_service_request_InverterInfo(sck, &si_sender, slen);
                break;
        case RequestTypeSetListener:
                ime_service_request_Listener(sck, &si_sender, slen, request);
                break;
        case RequestTypeInverterState:
                ime_service_request_InverterState(sck, &si_sender, slen);
                break;
        case RequestTypeGridState:
                ime_service_request_GridState(sck, &si_sender, slen);
                break;
        case RequestTypeInverterInfoJson:
                ime_service_request_InfoJson(sck, &si_sender, slen);
                break;
        default:
                ime_service_request_default(sck, &si_sender, slen);
    }
    bzero(srvbuffer, SRV_BUFFER_SIZE);

    return 0;
}
