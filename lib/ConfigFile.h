/*
 ============================================================================
 Name        : ConfigFile.h
 Author      : mimi
 Version     : 01.2023
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : lista parametrów odczytywanych z pliku konfiguracyjnego
 ============================================================================
 */
#ifndef CONFIGFILE_H_
#define CONFIGFILE_H_

#include <stdint.h>
#define CONFIG_MAXVALUELEN 	28

#define	CONFIG_DELIMITER1	' '
#define	CONFIG_DELIMITER2	'\t'

#define nullptr 0

struct Config
{
	char *key;
	char value[CONFIG_MAXVALUELEN];
};


struct ConfigList
{
	uint16_t size;
	struct Config (*List)[];
};

void ConfigClearValue(struct Config (*List)[], uint16_t list_size);
int ConfigFileRead(const char *config_filename, struct Config List[], uint16_t list_size);

#endif /* CONFIGFILE_H_ */
