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


enum BS_RETURN_CODE {
	BS_RETURN_CODE_OK = 0,
	BS_RETURN_CODE_QUIT,
	BS_RETURN_CODE_REFRESH,
	BS_RETURN_ERR_INADDR,
	BS_RETURN_ERR_SOCK,
	BS_RETURN_ERR_SOCK_CONN,
	BS_RETURN_ERR_SOCK_SEND,
	BS_RETURN_ERR_SOCK_RECV,
	BS_RETURN_ERR_BUSY,
	BS_RETURN_ERR_NOCMD,
	BS_RETURN_TIMEOUT,
	BS_RETURN_ERR_ERROR,
	BS_RETURN_CODE_END
};


#endif /* IME_MAIN_H_ */
