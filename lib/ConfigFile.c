/*
 ============================================================================
 Name        : ConfigFile.c
 Author      : mimi
 Version     : 01.2023
 Copyright   : GNU GENERAL PUBLIC LICENSE
 Description : odczyt parametrów z pliku konfiguracyjnego
 ============================================================================
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "ConfigFile.h"

#define CONFIGFILEREAD_DBG 0



static
char * ConfigParseSetValue(char *value_dsc, const char *line, uint16_t max_len)
{
    char c;
    const char *vbeg=0;
    int i;
    vbeg = 0;
    for(; *(line)!=0; line++)
    {
        c = *(line);
        if(vbeg==0) {
            if(c==CONFIG_DELIMITER1 || c==CONFIG_DELIMITER2)  vbeg = line;
        }
        else if(c!=CONFIG_DELIMITER1 && c!=CONFIG_DELIMITER2) break;
    }
    
    max_len--;
    for(i=0; i<max_len; i++, line++)
    {
        c = *(line);
        if(c==13 || c==10 || c==0) break;
        *(value_dsc+i) = c;
    }
    *(value_dsc+i) = 0;
    return value_dsc;
}

void ConfigClearValue(struct Config (*List)[], uint16_t list_size)
{
    int	 i;
    for(i=0; i<list_size; i++)
    {
        (*List)[i].value[0] = 0;
    }
}


int ConfigFileRead(const char *config_filename, struct Config List[], uint16_t list_size)
{
    FILE *confile;
   
    if(CONFIGFILEREAD_DBG) puts(config_filename);
    
    confile = fopen(config_filename, "r");
    if (!confile) return -1;

    /* read lines from file using POSIX function getline */
    char *line = NULL;
    size_t len = 0;
    int	 i;
    while(getline(&line, &len, confile) != -1)
    {
        while(isspace(*line)) line++;
        if(line[0]!='#')  // parse line
        {
            if(CONFIGFILEREAD_DBG) printf(line);
            for(i=0; i<list_size; i++)
            {
                if( strstr(line, List[i].key) == line )
                {
                    if(CONFIGFILEREAD_DBG) printf(" key %i  %s\n", i, List[i].key);
                    ConfigParseSetValue( List[i].value, line, CONFIG_MAXVALUELEN );
                    break;
                }
            }
        }
    } 
    
    fclose(confile); 
        
    return 0;
}
