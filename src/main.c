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
#include "ImeInverter.h"
#include "ImeConfig.h"
#include "ImeService.h"

volatile int run = 1;
uint8_t  print_debug_info;

static
void signal_term(int signal)
{
    run = 0;
}


int main(int argc, char**argv) {

    int retval, v;
    print_debug_info = 0;
    
    if(argc>1)
    {
        if(strstr(argv[1],"-d"))     print_debug_info = 2;
    }
    
    openlog("slog", LOG_PID|LOG_CONS, LOG_USER);

    signal(SIGINT, signal_term);
    signal(SIGTERM, signal_term);
    
    ime_inverter_init();
    
    retval = config_ReadFile();
    
    if(retval !=IME_RETURN_CODE_OK) {
        closelog();
        return EXIT_FAILURE;
    }

    syslog (LOG_INFO, "InverterBroker Start");
    if(print_debug_info) puts("InverterBroker Start");

    ime_inverter_thread_create();
    ime_service_start();

    while(run)
    {
        v = ime_service_thread();
        if(v==IME_RETURN_CODE_QUIT) break;
        if(v==IME_RETURN_ERR_ERROR) break;
    }

    run = 0;
    ime_service_stop();
    ime_inverter_thread_end();
    
    syslog (LOG_INFO, "InverterBroker Stop");
    closelog();
  
    return EXIT_SUCCESS;
}

