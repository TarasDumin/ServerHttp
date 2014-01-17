#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <stdio.h>
#include <time.h>
#include "auxiliaryFunction.h"
#include "debugLogging.h"
#include "workWithSockets.h"

#define SUCCESS             (1)
#define ERROR_RUN           (-1)
#define SIGINT              (2)

//purpose: get current time
//return: string of current time
char* getTimeS(void){
    char buffer[LENGTH_STRING_TIME];
    char *tmpTime;
    int lenTime = 0;
    time_t ticks = time(NULL);
    
    strcpy(buffer, ctime(&ticks));
    lenTime = strlen(buffer);
    tmpTime = malloc((lenTime + 1) * sizeof(char));
    strcpy(tmpTime, buffer);
    tmpTime[lenTime - 1] = 0;

    return tmpTime;
}


//purpose: convert all symbol of string to lower register
//parameter:
//     strBefor - string befor processing
//     strAfter - string after processing
void convertToLower(char *strBefor, char *strAfter) 
{
    int i;
    for(i=0; i < strlen(strBefor) + 1 ; i++) 
    {
        strAfter[i] = tolower(strBefor[i]);
    }
}
//purpose: mark that memory was freed
void freeNew(char **member)
{
    if(*member)
    {
        free(*member);    
    }
    *member = NULL;
    return;
}
//purpose: stop server when need
int doesServerHaveToWorkCommand(void)
{
    int result = 0;
    int countSymbol = 0;
    char buffer[MAX_LENGTH_TEXT];

    WRITE_LOG("Start | %s\n", " ");

    do
    {
        buffer[countSymbol] = 0;
        countSymbol = readWithTimeOut(STDIN, buffer, sizeof(char) * (MAX_LENGTH_TEXT - 1), 0);
        if(countSymbol == ERROR_RUN)
        {
            result = ERROR_RUN;
        }
    }while(countSymbol > 0);
    //printf("buffer: %s\n", buffer);
    if(!strcmp(buffer, "out\n") &&
       result != ERROR_RUN)
    {
        result = SUCCESS; 
    }
    WRITE_LOG("End | result: %d\n", result);
    return result;
}

//purpose: stop server when need
int doesServerHaveToWorkConfig(void)
{
    char string[MAX_LENGTH_TEXT] = {0};
    FILE *commandFile;
    int result = 0;
    
    WRITE_LOG("Start | %s\n", " ");

    commandFile = fopen("command", "r");
    if (commandFile) 
    {   
        fread(string, MAX_LENGTH_TEXT, sizeof(char), commandFile);
        fclose(commandFile);  
        result = !memcmp("exit", string, LENGTH_EXIT);
        
    }else{
        WRITE_LOG("Errno. Can't open command file? error:  %d\n", errno);
        result = ERROR_RUN;
    }

    WRITE_LOG("End | result: %d\n", result);
    return result;
}

static int exitCycle = 1;
int checkExit(void)
{
    return exitCycle;
}
//purpose: stop server when need
void sig_handler(int signo)
{
    if (signo == SIGINT)
    {
        exitCycle = 0;
    }
}
