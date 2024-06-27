/*
 ============================================================================
 Name        : main.c
 Author      : mimi
 Version     :
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : Inverter Broker Service
 ============================================================================
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <syscall.h>
#include <syslog.h>
#include <stdint.h>

#include <sys/socket.h>
#include <unistd.h>
#include <netdb.h>

#include "Ime.h" 
#include "threadInverter.h"
#include "Config.h"
#include "BrokerService.h"

volatile int run = 1;
uint8_t  print_debug_info;

static
void signal_term(int signal)
{
    run = 0;
}


int main(int argc, char**argv) {

    int rc;
    print_debug_info = 0;
    
    if(argc>1)
    {
        if(strstr(argv[1],"-d"))     print_debug_info = 2; 
    }
    
    openlog("slog", LOG_PID|LOG_CONS, LOG_USER);

    signal(SIGINT, signal_term);
    signal(SIGTERM, signal_term);
    
    thread_inverter_init();
    
    rc = config_ReadFile();
    if(rc !=IME_RETURN_CODE_OK) {
        closelog();
        return EXIT_FAILURE;
    }

    syslog (LOG_INFO, "InverterBroker Start");
    if(print_debug_info) puts("InverterBroker Start");

    thread_inverter_create();
    broker_service_start();
    
    while(run)
    {
        rc = broker_service_thread();
        if(rc==IME_RETURN_CODE_QUIT) break;
        if(rc==IME_RETURN_ERR_ERROR) break;
    }

    run = 0;
    broker_service_stop();
    thread_inverter_end();
    
    syslog (LOG_INFO, "InverterBroker Stop");
    closelog();
  
    return EXIT_SUCCESS;
}

