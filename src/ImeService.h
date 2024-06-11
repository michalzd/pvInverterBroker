/*
 ============================================================================
 Name        : ImeService.h
 Author      : mimi
 Version     : 01.2003
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : wątek servisu  
 ============================================================================
 */

#ifndef IMESERVICE_H_
#define IMESERVICE_H_

/*
 * threads run
 */
extern volatile int run;
extern uint8_t  print_debug_info;

int ime_service_start();
void ime_service_stop();

int ime_service_thread();


#endif /* IMESERVICE_H_ */
