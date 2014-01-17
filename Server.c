#include <stdlib.h>
#include <stdio.h>
#include <netdb.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "Server.h"
#include "request.h"
#include "workWithSockets.h" 
#include "debugLogging.h"
#include "auxiliaryFunction.h"
#include "accessLoggin.h"

int clientServerInteraction(int deskription)
{
    HTTP_REQUEST *currentReq;
    char message[LENGTH_STRING] = {0};
    char* time = NULL;
    int result;

    WRITE_LOG("start | Deskription: %d\n", deskription);
    
   
    do
    {
        currentReq = createRequestObject();
        //printf("#######################################\n");//for debug
        time = getTimeS();
        do
        {
            
            //printf("///////////////////////////////////////////////////////");//for debug
            result = readWithTimeOut(deskription, message, LENGTH_STRING, currentReq->timeOfKeapAlive);
             
            if(result > 0)
            {
                if((HTTP_200_OK != currentReq->status) || 
                   (!parseParameters(currentReq, message, result)))
                {
                    break;  
                }
            }
            else if(result == ERROR_RUN)
            {
                break;
            }
            //printf("************************************\n");//for debug
        }while(result == LENGTH_STRING - 1 && 
               result != TIME_OUT);
        if(result != TIME_OUT)
        {
            currentReq->MethodHTTP(currentReq, deskription); //
        }
        
        if(isUseInLog("HTTPStatus:"))
        {
            getLogeW3CLog()->HTTPStatus = currentReq->status;
        }
        if(isUseInLog("Date:"))
        {
            if(!isEmptyLogDate(getLogeW3CLog()->Date))
            {
                free(getLogeW3CLog()->Date);
            }
            getLogeW3CLog()->Date = time;
        }
        deleteRequest(currentReq);
        if(result != TIME_OUT)
        {
            writeInLogFile();
        }
        clearW3CLog();
    }while(result != TIME_OUT &&
           currentReq->isKeapAliveUsing);
    
    //printf("------------------------------------------------------------------\n");//for debug
    close(deskription);
    WRITE_LOG("End | Deskription: %d | result: %d\n", deskription, result);
    return result;
}




int main(void)
{
    int deskriptionS = 0, deskriptionC = 0;
    int pid = 0;   

    initW3CLog();  
    startDebug();
    printf("Server is working now\n");
    WRITE_LOG("Server is working now %s", "");
    deskriptionS = connectToSocket();
    if(deskriptionS == ERROR_RUN)
    {
        exit(1);
    }

    while(checkExit())
    {
        deskriptionC = acceptWithLogger(deskriptionS);
        if(deskriptionC != TIME_OUT)
        {
            if(deskriptionC == ERROR_RUN)
            {
                exit(1);
            }
            
            pid = fork();
            if (pid < 0)
            {
                WRITE_LOG("Error. Fork was failed%s\n", " ");
	        exit(1);
            }
            if(pid != 0)
            {        
                close(deskriptionS);
                if(clientServerInteraction(deskriptionC) == ERROR_RUN)
                {
                    exit(1);
                }
                return (EXIT_SUCCESS);//break;
            }
            close(deskriptionC);
        }   
        if (signal(SIGINT, sig_handler) == SIG_ERR)
        {
            WRITE_LOG("Error. can't catch SIGINT%s\n", " ");
        }
        if(doesServerHaveToWorkCommand() > 0)
        {
            break;
        }
        if(doesServerHaveToWorkConfig() > 0)
	{
            break;
	}
    }
    if(pid == 0)
    {
        printf("Server is stopped\n");
        close(deskriptionS);
        
        endDebug();
        WRITE_LOG("Server is stoped%s", "");
    }
    return (EXIT_SUCCESS);
}
         


