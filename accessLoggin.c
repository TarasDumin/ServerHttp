#include <sys/sem.h> 
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "accessLoggin.h"

#define LENGTH_SERVERNAME 8

static W3C_LOG W3CLog;
static int semIdW3C;
static FILE *debugLogin;

YES_NO_QUESTION isUseInLog(char *Key)
{
    YES_NO_QUESTION result = NO_Q;
    char parameter[MAX_LENGTH_STRING];
    getValueForKeyString("accessLoggin.txt", Key, parameter);
    if(parameter[0] == 'Y')
    {
        result = YES_Q;
    }
    return result;
}

void initW3CLog(void)
{
    semIdW3C = semget(IPC_PRIVATE, 1, 0xfff);
    W3CLog.ServerName = "HW/1.0.0";
    W3CLog.Date = &W3CLog.ServerName[LENGTH_SERVERNAME];// W3CLog.ServerName[LENGTH_SERVERNAME] => default pointer for empty of pointer
    W3CLog.Time = &W3CLog.ServerName[LENGTH_SERVERNAME];
    W3CLog.ClientIPAddress = &W3CLog.ServerName[LENGTH_SERVERNAME];
    W3CLog.UserName = &W3CLog.ServerName[LENGTH_SERVERNAME];
    W3CLog.ServiceNameAndInstanceNumber = &W3CLog.ServerName[LENGTH_SERVERNAME];
    W3CLog.ServerIPAddress = &W3CLog.ServerName[LENGTH_SERVERNAME];           
    W3CLog.ServerPort = &W3CLog.ServerName[LENGTH_SERVERNAME];                  
    W3CLog.Method = &W3CLog.ServerName[LENGTH_SERVERNAME];                     
    W3CLog.URIStem = &W3CLog.ServerName[LENGTH_SERVERNAME];                    
    W3CLog.URIQuery = &W3CLog.ServerName[LENGTH_SERVERNAME];                    
    W3CLog.HTTPStatus = 0;
    W3CLog.Win32Status = &W3CLog.ServerName[LENGTH_SERVERNAME];
    W3CLog.BytesSent = 0;
    W3CLog.BytesReceived = &W3CLog.ServerName[LENGTH_SERVERNAME];
    W3CLog.TimeTaken = &W3CLog.ServerName[LENGTH_SERVERNAME];
    W3CLog.ProtocolVersion = &W3CLog.ServerName[LENGTH_SERVERNAME];
    W3CLog.Host = &W3CLog.ServerName[LENGTH_SERVERNAME];
    W3CLog.UserAgent = &W3CLog.ServerName[LENGTH_SERVERNAME];
    W3CLog.Cookie = &W3CLog.ServerName[LENGTH_SERVERNAME];
    W3CLog.Referrer = &W3CLog.ServerName[LENGTH_SERVERNAME];
    W3CLog.ProtocolSubstatus = &W3CLog.ServerName[LENGTH_SERVERNAME];
    return;
}

W3C_LOG *getLogeW3CLog(void)
{
    return &W3CLog;
}

void writeInLogFile(void)
{
    struct sembuf sem_command;  
    //
    sem_command.sem_op = 0;   // waits for semval == 0
    semop(semIdW3C, &sem_command, 1);
    sem_command.sem_op = 1;   // increment semval by 1
    semop(semIdW3C, &sem_command, 1);
    debugLogin = fopen("logW3C.txt","a+");
    fprintf(debugLogin, "%s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s %s", 
                                                              W3CLog.ServerName,      W3CLog.ClientIPAddress,
                                                              W3CLog.Time,            W3CLog.Date,
                                                              W3CLog.UserName,        W3CLog.ServiceNameAndInstanceNumber, 
                                                              W3CLog.ServerIPAddress, W3CLog.ServerPort, 
                                                              W3CLog.Method,          W3CLog.URIStem, 
                                                              W3CLog.URIQuery,        W3CLog.Win32Status,      
                                                              W3CLog.BytesReceived,   W3CLog.TimeTaken, 
                                                              W3CLog.ProtocolVersion, W3CLog.Host, 
                                                              W3CLog.UserAgent,       W3CLog.Cookie, 
                                                              W3CLog.Referrer,        W3CLog.ProtocolSubstatus);
    if(isUseInLog("HTTPStatus:"))
    {
        fprintf(debugLogin, "%d ", W3CLog.HTTPStatus);
    }
    if(isUseInLog("BytesSent:"))
    {
        fprintf(debugLogin, "%d ", W3CLog.BytesSent);
    }
    fprintf(debugLogin, "\n");

    fclose(debugLogin);
    sem_command.sem_op = -1;   // decrement semval by 1
    semop(semIdW3C, &sem_command, 1);
    return;
}

void clearW3CLog(void)
{
    if(!isEmptyLogDate(W3CLog.Date))
    {
        free(W3CLog.Date);
        W3CLog.Date = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    if(!isEmptyLogDate(W3CLog.Time))
    {
        free(W3CLog.Time);
        W3CLog.Time = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    if(!isEmptyLogDate(W3CLog.UserName))
    {
        free(W3CLog.UserName);
        W3CLog.UserName = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    if(!isEmptyLogDate(W3CLog.ServiceNameAndInstanceNumber))
    {
        free(W3CLog.ServiceNameAndInstanceNumber);
        W3CLog.ServiceNameAndInstanceNumber = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    if(!isEmptyLogDate(W3CLog.ServerPort))
    {
        free(W3CLog.ServerPort);
        W3CLog.ServerPort = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }               
    if(!isEmptyLogDate(W3CLog.Method))
    {
        free(W3CLog.Method);
        W3CLog.Method = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    if(!isEmptyLogDate(W3CLog.URIStem))
    {
        free(W3CLog.URIStem);
        W3CLog.URIStem = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    if(!isEmptyLogDate(W3CLog.URIQuery))
    {
        free(W3CLog.URIQuery);
        W3CLog.URIQuery = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    W3CLog.HTTPStatus = 0;
    if(!isEmptyLogDate(W3CLog.Win32Status))
    {
        free(W3CLog.Win32Status);
        W3CLog.Win32Status = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    W3CLog.BytesSent = 0;
    if(!isEmptyLogDate(W3CLog.BytesReceived))
    {
        free(W3CLog.BytesReceived);
        W3CLog.BytesReceived = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    if(!isEmptyLogDate(W3CLog.TimeTaken))
    {
        free(W3CLog.TimeTaken);
        W3CLog.TimeTaken = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    if(!isEmptyLogDate(W3CLog.ProtocolVersion))
    {
        free(W3CLog.ProtocolVersion);
        W3CLog.ProtocolVersion = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    if(!isEmptyLogDate(W3CLog.Host))
    {
        free(W3CLog.Host);
        W3CLog.Host = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    if(!isEmptyLogDate(W3CLog.UserAgent))
    {
        free(W3CLog.UserAgent);
        W3CLog.UserAgent = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    if(!isEmptyLogDate(W3CLog.Cookie))
    {
        free(W3CLog.Cookie);
        W3CLog.Cookie = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    if(!isEmptyLogDate(W3CLog.Referrer))
    {
        free(W3CLog.Referrer);
        W3CLog.Referrer = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }
    if(!isEmptyLogDate(W3CLog.ProtocolSubstatus))
    {
        free(W3CLog.ProtocolSubstatus);
        W3CLog.ProtocolSubstatus = &W3CLog.ServerName[LENGTH_SERVERNAME];
    }

    return;
}

int isEmptyLogDate(char* dateLog)
{
    return dateLog == &W3CLog.ServerName[LEN_SERVER_NAME] ? TRUE : FALSE;
}
