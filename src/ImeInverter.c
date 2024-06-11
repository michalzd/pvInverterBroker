/*
 ============================================================================
 Name        : ImeInverter.c
 Author      : mimi
 Version     : 01.2003
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : wątek komunikacji z inverterem PV
 ============================================================================
 */
#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <syslog.h>
#include <arpa/inet.h>
#include <BrokerService.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netdb.h>
#include <resolv.h>
#include <unistd.h>
#include <time.h> 
#include <sys/wait.h>
#include <syscall.h>
#include <sched.h>
#include <linux/sched.h> 
#include <string.h>
#include <poll.h>

#include "Ime.h"
#include "ImeInverter.h"
#include "ImeService.h"
#include "ImeConfig.h"
#include "LoggerSofar.h"

volatile struct Inverter  inverterState;
volatile struct Grid	  gridState;

struct InverterPV InverterInputPV1;
struct InverterPV InverterInputPV2;

struct tm*   time_info;


/*
 * 2 kB stack for thread ( na 1kB wieszał się )
 */
#define THREAD_STACK_SIZE 2048

static int fd[2];
pid_t inverter_thread_pid;
int   inverter_thread_sck;
volatile int refreshtimer;


static inline void ime_inverter_refresh()    
{
    logger_sofar_refresh();  // do podmiany na funkcje odpowiednią dla invertera
}

static int ime_inverter_msg_send();

int ime_inverter_init()
{
	inverterState.state = InverterStateInit;
	inverterState.activepower = 0;
	return ime_inverter_clear();
}

int ime_inverter_clear()
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


pid_t	ime_inverter_thread_create()
{
    int   flags;
    uint8_t *stackTop;                 /* End of stack buffer */

    socketpair(PF_LOCAL, SOCK_STREAM, 0, fd);
    if(fd[parentsocket]==0) return(-2);

    //stackTop = threadStack + THREAD_STACK_SIZE-1;  /* Assume stack grows downward */
    uint8_t *stack;                    /* Start of stack buffer */
    stack = malloc(THREAD_STACK_SIZE);
    if (stack == NULL) return 0;
    stackTop = stack + THREAD_STACK_SIZE;

    flags = CLONE_VM | CLONE_SIGHAND;
    inverter_thread_pid = clone( &ime_inverter_thread, stackTop, flags, (void*) NULL );
    inverter_thread_sck = fd[childsocket];
    return inverter_thread_pid;
}


void  ime_inverter_thread_end()
{
    waitpid(inverter_thread_pid, NULL, __WCLONE);    /* Wait for child thread */
    if(fd[childsocket]) close(fd[childsocket]);
    if(fd[parentsocket]) close(fd[parentsocket]);
}




int  ime_inverter_thread_sck()
{
    return fd[parentsocket];
}

int ime_inverter_thread( void *none)
{
    struct pollfd poolfd;
    int ret; 
    int refreshtimer;
    time_t timer;
    
    while(run)
    {
        poolfd.fd = inverter_thread_sck;
	poolfd.events = POLLIN;
	ret = poll(&poolfd, 1, 15000); // 15 second for timeout
	
        refreshtimer+=15;
        if(run && refreshtimer >= sofarLogger.refreshtime )
        { 
            timer = time(NULL);
            time_info = localtime(&timer);
            refreshtimer = 0;
            inverterState.tmsec = time_info->tm_sec;
            inverterState.tmmin = time_info->tm_min;
            inverterState.tmhour = time_info->tm_hour;
            inverterState.tmweekday = time_info->tm_wday;
            ime_inverter_refresh();
            ime_inverter_msg_send();
        }
        
    }
    
}

static
int ime_inverter_msg_send()
{
    struct ServiceResponseInverterInfo info;
    
    info.InverterState.tmsec = inverterState.tmsec;
    info.InverterState.tmmin = inverterState.tmmin;
    info.InverterState.tmhour = inverterState.tmhour;
    info.InverterState.tmweekday = inverterState.tmweekday;
    info.InverterState.state = inverterState.state;
    info.InverterState.activepower = htons(inverterState.activepower);
    info.InverterState.averagepower = htons(inverterState.averagepower);
    info.PV1.voltage = htons(InverterInputPV1.voltage);
    info.PV1.current = htons(InverterInputPV1.current);
    info.PV2.voltage = htons(InverterInputPV2.voltage);
    info.PV2.current = htons(InverterInputPV2.current);

    info.Grid.Rvoltage = htons(gridState.Rvoltage);
    info.Grid.Rcurrent = htons(gridState.Rcurrent);
    info.Grid.Svoltage = htons(gridState.Svoltage);
    info.Grid.Scurrent = htons(gridState.Scurrent);
    info.Grid.Tvoltage = htons(gridState.Tvoltage);
    info.Grid.Tcurrent = htons(gridState.Tcurrent);
    info.Grid.voltage  = htons(gridState.voltage);
    info.Grid.maxphase = gridState.maxphase;
    info.Grid.phaseOverV = gridState.phaseOverV;
     
    return write(inverter_thread_sck, &info, sizeof(struct ServiceResponseInverterInfo) );
}