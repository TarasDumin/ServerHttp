#include <sys/sem.h> 
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include "debugLogging.h"

#define LENGTH_LOG_STRINGTH 256
static FILE *debugLogin;
static int semId;

void startDebug()
{
    //debugLogin = fopen("debugLogFile.txt","w");//is being cleaned previous debug
    //fclose(debugLogin);
    semId = semget(IPC_PRIVATE, 1, 0xfff);
    return;
}

void writeLog(const char *nameFile,const char *nameFunc, int line, char *text)
{
    struct sembuf sem_command; 
    time_t ticks = time(NULL);
    //
    sem_command.sem_op = 0;   // waits for semval == 0
    semop(semId, &sem_command, 1);
    sem_command.sem_op = 1;   // increment semval by 1
    semop(semId, &sem_command, 1);

    debugLogin = fopen("debugLogFile.txt","a+");
    fflush(stdout);
    fprintf(debugLogin, "---pid: %d | File: %s | Func: %s | line: %d -- %s%s\n", getpid(), nameFile, nameFunc, line, ctime(&ticks), text);
    fclose(debugLogin);

    sem_command.sem_op = -1;   // decrement semval by 1
    semop(semId, &sem_command, 1);
    return;
}

void endDebug()
{
    return;
}


