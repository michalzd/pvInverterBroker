/*
 ============================================================================
 Name        : Ime.h
 Author      : mimi
 Version     :
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : 
 ============================================================================
 */

#ifndef IME___H_
#define IME___H_

#include <stdlib.h>
#include <stdint.h>

#define  SERVICE_PORT_RANGE_MIN 8800
#define  SERVICE_PORT_RANGE_MAX 8890


enum IME_RETURN_CODE {
	IME_RETURN_CODE_OK = 0,
	IME_RETURN_CODE_QUIT,
	IME_RETURN_CODE_REFRESH,
	IME_RETURN_ERR_INADDR,
	IME_RETURN_ERR_SOCK,
	IME_RETURN_ERR_SOCK_CONN,
	IME_RETURN_ERR_SOCK_SEND,
	IME_RETURN_ERR_SOCK_RECV,
	IME_RETURN_ERR_BUSY,
	IME_RETURN_ERR_NOCMD,
	IME_RETURN_TIMEOUT,
	IME_RETURN_ERR_ERROR,
	IME_RETURN_CODE_END
};


#endif /* IME_MAIN_H_ */
