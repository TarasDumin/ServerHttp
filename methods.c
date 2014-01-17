#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "debugLogging.h"
#include "methods.h"
#include "auxiliaryFunction.h"
#include "accessLoggin.h"
#include "workWithSockets.h" 

#define MAX_LENGTH_HEADER 1024
#define MAX_LENGTH_LINE 256
#define RESPONSE_ERROR(buffer, version, status) sprintf(buffer, "HTTP/1.%d %d\r\nContent-Length: 37\r\n\r\n<center><B>%d Error</B></center>\r\n",version , status, status)

int getLengthFile(HTTP_REQUEST*);
//purpose: create header of the response
//parameter:
//    currentReq - info about request.
//    pheader - pointer for header of the response
void createHeader(HTTP_REQUEST *currentReq, char** pheader)
{
    char *header = NULL;
    char buffer[MAX_LENGTH_HEADER];
    char oneParameter[MAX_LENGTH_LINE];
    char *time = getTimeS();
    int lenHeader = 0;
    int lenParametr = 0;

    WRITE_LOG("Start | status: %d\n", currentReq->status);

    sprintf(buffer, "HTTP/1.%d %d\r\n", currentReq->version, currentReq->status);
    lenHeader = strlen(buffer);
    //server
    if(currentReq->status == HTTP_200_OK)
    {
        lenParametr = sprintf(oneParameter,"Server: HW/1.0.0\r\n");
        if((lenHeader + lenParametr) < MAX_LENGTH_HEADER - 1)//reserv for symbol end of string and end of header 
        { 
            strcat(buffer,oneParameter); 
            lenHeader += lenParametr;
        }
        else
        {
            currentReq->status = HTTP_431_REQUEST_HEADER_FIELDS_TOO_LARGE;
            RESPONSE_ERROR(buffer, currentReq->version, currentReq->status);
            return;
        }
        //date
        lenParametr = sprintf(oneParameter,"Date: %s\r\n", time);
        if((lenHeader + lenParametr) < MAX_LENGTH_HEADER)
        { 
            strcat(buffer,oneParameter); 
            lenHeader += lenParametr;
        }
        else
        {
            currentReq->status = HTTP_431_REQUEST_HEADER_FIELDS_TOO_LARGE;
            RESPONSE_ERROR(buffer, currentReq->version, currentReq->status);
            return;
        }
        //Content-Type
        if(currentReq->URI)
        {
            char *pType = strrchr(currentReq->URI, '.');
            if(pType)
            {
                int lenType = strlen(++pType);
                if(0 < lenType &&
                   MAX_LENGTH_TYPE > lenType )
                {
                    char typeConteType[MAX_LENGTH_CONTENT_TYPE];
                    char typeFile[MAX_LENGTH_TYPE + 1];//type + ":"

                    convertToLower(pType, typeFile);
                    strcat(typeFile, ":");

                    if(SUCCESS == getValueForKeyString("dictionaryType.txt", typeFile, typeConteType))
                    {
                        lenParametr = sprintf(oneParameter,"Content-Type: %s\n", typeConteType);
                        if((lenHeader + lenParametr) < MAX_LENGTH_HEADER)
                        {
                            strcat(buffer,oneParameter);
                            lenHeader += lenParametr;
                        }
                        else
                        {
                            currentReq->status = HTTP_431_REQUEST_HEADER_FIELDS_TOO_LARGE;
                            RESPONSE_ERROR(buffer, currentReq->version, currentReq->status);
                            return;
                        }
                    }
                    //
                }
            }
        }
        //Connection: keep-alive
        if(currentReq->isKeapAliveUsing)
        {
            lenParametr = sprintf(oneParameter,"Connection: keep-alive\r\n");
            if((lenHeader + lenParametr) < MAX_LENGTH_HEADER)
            { 
                strcat(buffer,oneParameter); 
                lenHeader += lenParametr;
            }
            else
            {
                currentReq->status = HTTP_431_REQUEST_HEADER_FIELDS_TOO_LARGE;
                RESPONSE_ERROR(buffer, currentReq->version, currentReq->status);
                return;
            }
        }
        //chunk
        if(currentReq->isChunkUsing)
        {
            lenParametr = sprintf(oneParameter,"Transfer-Encoding: chunked\r\n");
            if((lenHeader + lenParametr) < MAX_LENGTH_HEADER)
            { 
                strcat(buffer,oneParameter); 
                lenHeader += lenParametr;
            }
            else
            {
                currentReq->status = HTTP_431_REQUEST_HEADER_FIELDS_TOO_LARGE;
                RESPONSE_ERROR(buffer, currentReq->version, currentReq->status);
                return;
            }
        }
        else 
        { 
            if(currentReq->lenBodyResponse > 0)
            {
                lenParametr = sprintf(oneParameter,"Content-Length: %d\r\n", currentReq->lenBodyResponse);
                if((lenHeader + lenParametr) < MAX_LENGTH_HEADER)
                { 
                    strcat(buffer,oneParameter); 
                    lenHeader += lenParametr;
                }
                else
                {
                    currentReq->status = HTTP_431_REQUEST_HEADER_FIELDS_TOO_LARGE;
                    RESPONSE_ERROR(buffer, currentReq->version, currentReq->status);
                    return;
                }
            }
        }
    }
    else
    {
        
        sprintf(oneParameter, "Content-Length: 37\r\n\r\n<center><B>%d Error</B></center>\r\n", currentReq->status);
        strcat(buffer, oneParameter);
    }
    strcat(buffer,"\r\n"); 
    header = malloc(strlen(buffer) + 1);
    if(!header)
    {
        WRITE_LOG("Error. Memory isn't allocated for header %s\n", " ");
        exit(1);
    }
    strcpy(header, buffer);
    *pheader = header;
    free(time);
    WRITE_LOG("End | status: %d\n", currentReq->status);
    return;
}
//purpose: get length of file
//parameter:
//    currentReq - object for current message of request.
//return: size of file
int getLengthFile(HTTP_REQUEST *currentReq)
{
    int sizeFile = 0;
    FILE *invitedFile = NULL;
    struct stat statistic;
       
    WRITE_LOG("Start | currentReq->URI: %p\n", currentReq->URI);
    stat(currentReq->URI, &statistic);
    if((statistic.st_mode & S_IFMT) == S_IFDIR)
    {
        currentReq->status = HTTP_404_NOT_FOUND;
    }
    else if(strstr(currentReq->URI, "/../")) 
    {
        currentReq->status = HTTP_400_BAD_REQUEST;
    }
    else
    {
        invitedFile = fopen(currentReq->URI, "rb");
    }
    
    if (invitedFile)
    {  
        //get length file
        fseek(invitedFile, 0L, SEEK_END);
        sizeFile = ftell(invitedFile);
        fseek(invitedFile, 0L, SEEK_SET);
        fclose(invitedFile); 
    }else{
        if(errno == 2) 
        {
            currentReq->status = HTTP_404_NOT_FOUND;
        }
        else if(errno == 4)
        {
            currentReq->status = HTTP_423_LOCKED;
        }
        sizeFile = ERROR_RUN;
    }
    WRITE_LOG("End | sizeFile: %d | status: %d \n", sizeFile, currentReq->status); 
    return sizeFile;
}
//purpose: request a representation of the resource
//parameter:
//    currentReq - object for current message of request.
//    messageReq - current message of request.
//return: size of file
int readFile(HTTP_REQUEST *currentReq, char **bodyFile, int lenBuffer, int offset)
{
    int sizeFile = 0;
    char *body;
    FILE *invitedFile = NULL;
    
    WRITE_LOG("Start | currentReq->URI: %p\n", currentReq->URI);
    
    invitedFile = fopen(currentReq->URI, "rb");
    if (invitedFile)
    {   
        //get length file
        if(lenBuffer == READ_FULL_FILE)
        {
            fseek(invitedFile, 0L, SEEK_END);
            sizeFile = ftell(invitedFile);
            fseek(invitedFile, 0L, SEEK_SET); 
        }
        else
        {
            fseek(invitedFile, offset, SEEK_SET);
            sizeFile = lenBuffer;
        }
        body = malloc(sizeFile * sizeof(char));
        if(!body)
        {
            sizeFile = ERROR_RUN;
            currentReq->status = HTTP_500_INTERNAL_SERVER_ERROR;
        }
        else
        {
            sizeFile = fread(body, sizeof(char), sizeFile, invitedFile);
            *bodyFile = body;
        }
        fclose(invitedFile); 
    }else{
        if(errno == 2) 
        {
            currentReq->status = HTTP_404_NOT_FOUND;
        }
        else if(errno == 4)
        {
            currentReq->status = HTTP_423_LOCKED;
        }
        sizeFile = ERROR_RUN;
    }

    WRITE_LOG("End | sizeFile: %d | status: %d \n", sizeFile, currentReq->status); 
    return sizeFile;
}
//purpose: request a representation of the resource.
//parameter:
//    currentReq - object for current message of request.
//    messageReq - current message of request.
void methodGet(HTTP_REQUEST *currentReq, int deskription)
{
    char *bodyFile;
    int readBytes = 0;
    WRITE_LOG("Start | status: %d | currentReq: %p | deskription: %d\n", currentReq->status, currentReq, deskription);
    //access log
    if(isUseInLog("Method:"))
    {
        if(!isEmptyLogDate(getLogeW3CLog()->Date))
        {
            free(getLogeW3CLog()->Date);
        }
        getLogeW3CLog()->Method = malloc(strlen("GET") + 1);
        sprintf(getLogeW3CLog()->Method,"GET");
    }

    currentReq->lenBodyResponse = getLengthFile(currentReq);
    createHeader(currentReq, &currentReq->header);
    currentReq->lenBodyResponse = strlen(currentReq->header);
    //access log
    if(isUseInLog("BytesSent:"))
    {
        getLogeW3CLog()->BytesSent += currentReq->lenResponse;
    }

    if(sendAll(deskription, currentReq->header, &currentReq->lenBodyResponse) == ERROR_RUN)
    {
        WRITE_LOG("Error. sendAll is failed%s \n", " ");
    }
    if(currentReq->status == HTTP_200_OK)
    {
        do
        {
            if(currentReq->isChunkUsing)
            {
                currentReq->lenBodyResponse = readFile(currentReq, &bodyFile, LENGTH_CHUNK, readBytes);
                readBytes += currentReq->lenBodyResponse;
            }
            else
            {
                currentReq->lenBodyResponse = readFile(currentReq, &bodyFile, READ_FULL_FILE, 0);
            }
            if(currentReq->lenBodyResponse != ERROR_RUN)
            {
                int lenOfLenChunk = 0;
                int lenOfEndChunk = 0;
                char partForChunk[LENGTH_PART_CHUNK];
                if(currentReq->isChunkUsing)
                {    
                    sprintf(partForChunk, "%x\r\n", currentReq->lenBodyResponse);
                    lenOfLenChunk = strlen(partForChunk);
                    lenOfEndChunk = PLACE_FOR_END_CHUNK;
                    currentReq->lenBodyResponse += lenOfLenChunk + lenOfEndChunk;
                }        
                currentReq->body = malloc(currentReq->lenBodyResponse * sizeof(char));
                if(!currentReq->body)
                {
                    currentReq->status = HTTP_500_INTERNAL_SERVER_ERROR;
                    freeNew(&currentReq->header);
                    createHeader(currentReq, &currentReq->header);
                    currentReq->body = currentReq->header;
                }
                else
                {
                    if(currentReq->isChunkUsing)
                    {
                        memcpy(currentReq->body, partForChunk, lenOfLenChunk);
                        memcpy(currentReq->body + currentReq->lenBodyResponse - lenOfEndChunk , "\r\n", lenOfEndChunk);
                    } 
                    memcpy(currentReq->body + lenOfLenChunk, bodyFile, currentReq->lenBodyResponse -  lenOfLenChunk - lenOfEndChunk);
                    freeNew(&bodyFile);
                    WRITE_LOG("Start | currentReq->URI: %p\n", currentReq->URI);
                }
            }
            else
            {
                WRITE_LOG("Error. Read file is failed%s \n", " ");
            }
            if(isUseInLog("BytesSent:"))
            {
                getLogeW3CLog()->BytesSent += currentReq->lenBodyResponse;
            }
            if(sendAll(deskription, currentReq->body, &currentReq->lenBodyResponse) == ERROR_RUN)
            {
                WRITE_LOG("Error. sendAll is failed%s \n", " ");
            }
            freeNew(&currentReq->body);
        }
        while(currentReq->isChunkUsing &&
              currentReq->lenBodyResponse > SIZE_CHUNK_WITHOUT_DATE);
    }
    
    WRITE_LOG("End | currentReq: %p | deskription: %d\n", currentReq, deskription);
    return;
}
//purpose: request only the headers for the resource
//parameter:
//    currentReq - object for current message of request.
//    messageReq - current message of request.
void methodHead(HTTP_REQUEST *currentReq, int deskription)
{   
    WRITE_LOG("Start | status: %d | currentReq: %p | deskription: %d\n", currentReq->status, currentReq, deskription);
    if(isUseInLog("Method:"))
    {
        getLogeW3CLog()->Method = malloc(strlen("HEAD") + 1);
        sprintf(getLogeW3CLog()->Method,"HEAD");
    }
    currentReq->lenBodyResponse = getLengthFile(currentReq);
    createHeader(currentReq, &currentReq->header);
    currentReq->lenResponse = strlen(currentReq->header);
    currentReq->body = malloc(currentReq->lenResponse * sizeof(char));
    memcpy(currentReq->body, currentReq->header, currentReq->lenResponse);
    freeNew(&currentReq->URI);
    if(sendAll(deskription, currentReq->body, &currentReq->lenResponse) == ERROR_RUN)
    {
        WRITE_LOG("Error. sendAll is failed%s \n", " ");
    }
    freeNew(&currentReq->body);
    WRITE_LOG("End | currentReq: %p | deskription: %d\n", currentReq, deskription);
    return;
}
//purpose: process the request body with the resource.
//parameter:
//    currentReq - object for current message of request.
//    messageReq - current message of request.
void methodPost(HTTP_REQUEST *currentReq, int deskription)
{
    WRITE_LOG("Start | status: %d | currentReq: %p | deskription: %d\n", currentReq->status, currentReq, deskription);
    if(isUseInLog("Method:"))
    {
        getLogeW3CLog()->Method = malloc(strlen("POST") + 1);
        sprintf(getLogeW3CLog()->Method,"POST");
    }
    currentReq->status = HTTP_501_NOT_IMPLEMENTED;
    currentReq->lenResponse = LENGTH_ISNT_IMPLEMENTED_RESPONSE;
    currentReq->body = malloc(currentReq->lenResponse * sizeof(char));
    RESPONSE_ERROR(currentReq->body, currentReq->version, currentReq->status);
    
    if(sendAll(deskription, currentReq->body, &currentReq->lenResponse) == ERROR_RUN)
    {
        WRITE_LOG("Error. sendAll is failed%s \n", " ");
    }
    freeNew(&currentReq->body);
    WRITE_LOG("End | currentReq: %p | deskription: %d\n", currentReq, deskription);
    return;
}
//purpose: create or update a new resource with the contents of the request body.
//parameter:
//    currentReq - object for current message of request.
//    messageReq - current message of request.
void methodPut(HTTP_REQUEST *currentReq, int deskription)
{
    WRITE_LOG("Start | status: %d | currentReq: %p | deskription: %d\n", currentReq->status, currentReq, deskription);
    if(isUseInLog("Method:"))
    {
        getLogeW3CLog()->Method = malloc(strlen("PUT") + 1);
        sprintf(getLogeW3CLog()->Method,"PUT");
    }
    currentReq->status = HTTP_501_NOT_IMPLEMENTED;
    currentReq->lenResponse = LENGTH_ISNT_IMPLEMENTED_RESPONSE;
    currentReq->body = malloc(currentReq->lenResponse * sizeof(char));
    RESPONSE_ERROR(currentReq->body, currentReq->version, currentReq->status);
    
    if(sendAll(deskription, currentReq->body, &currentReq->lenResponse) == ERROR_RUN)
    {
        WRITE_LOG("Error. sendAll is failed%s \n", " ");
    }
    freeNew(&currentReq->body);
    WRITE_LOG("End | currentReq: %p | deskription: %d\n", currentReq, deskription);
    return;
}
//purpose: remove the specified resource.
//parameter:
//    currentReq - object for current message of request.
//    messageReq - current message of request.
void methodDelete(HTTP_REQUEST *currentReq, int deskription)
{
    WRITE_LOG("Start | status: %d | currentReq: %p | deskription: %d\n", currentReq->status, currentReq, deskription);
    if(isUseInLog("Method:"))
    {
        getLogeW3CLog()->Method = malloc(strlen("DELETE") + 1);
        sprintf(getLogeW3CLog()->Method,"DELETE");
    }
    currentReq->status = HTTP_501_NOT_IMPLEMENTED;
    currentReq->lenResponse = LENGTH_ISNT_IMPLEMENTED_RESPONSE;
    currentReq->body = malloc(currentReq->lenResponse * sizeof(char));
    RESPONSE_ERROR(currentReq->body, currentReq->version, currentReq->status);
    
    if(sendAll(deskription, currentReq->body, &currentReq->lenResponse) == ERROR_RUN)
    {
        WRITE_LOG("Error. sendAll is failed%s \n", " ");
    }
    freeNew(&currentReq->body);
    WRITE_LOG("End | currentReq: %p | deskription: %d\n", currentReq, deskription);
    return;
}
//purpose: return the HTTP methods the specified resource supports.
//parameter:
//    currentReq - object for current message of request.
//    messageReq - current message of request.
void methodOptions(HTTP_REQUEST *currentReq, int deskription)
{
    WRITE_LOG("Start | status: %d | currentReq: %p | deskription: %d\n", currentReq->status, currentReq, deskription);
    if(isUseInLog("Method:"))
    {
        getLogeW3CLog()->Method = malloc(strlen("OPTIONS") + 1);
        sprintf(getLogeW3CLog()->Method,"OPTIONS");
    }
    currentReq->status = HTTP_501_NOT_IMPLEMENTED;
    currentReq->lenResponse = LENGTH_ISNT_IMPLEMENTED_RESPONSE;
    currentReq->body = malloc(currentReq->lenResponse * sizeof(char));
    RESPONSE_ERROR(currentReq->body, currentReq->version, currentReq->status);
    
    if(sendAll(deskription, currentReq->body, &currentReq->lenResponse) == ERROR_RUN)
    {
        WRITE_LOG("Error. sendAll is failed%s \n", " ");
    }
    freeNew(&currentReq->body);
    return;
}
//purpose: echo the received request.
//parameter:
//    currentReq - object for current message of request.
//    messageReq - current message of request.
void methodTrace(HTTP_REQUEST *currentReq, int deskription)
{
    WRITE_LOG("Start | status: %d | currentReq: %p | deskription: %d\n", currentReq->status, currentReq, deskription);
    if(isUseInLog("Method:"))
    {
        getLogeW3CLog()->Method = malloc(strlen("TRACE") + 1);
        sprintf(getLogeW3CLog()->Method,"TRACE");
    }
    currentReq->status = HTTP_501_NOT_IMPLEMENTED;
    currentReq->lenResponse = LENGTH_ISNT_IMPLEMENTED_RESPONSE;
    currentReq->body = malloc(currentReq->lenResponse * sizeof(char));
    RESPONSE_ERROR(currentReq->body, currentReq->version, currentReq->status);
    
    if(sendAll(deskription, currentReq->body, &currentReq->lenResponse) == ERROR_RUN)
    {
        WRITE_LOG("Error. sendAll is failed%s \n", " ");
    }
    freeNew(&currentReq->body);
    return;
}
//purpose: convert the connection to a transparent tcp/ip tunnel, usually to allow SSL/TLS through an unencrypted HTTP proxy.
//parameter:
//    currentReq - object for current message of request.
//    messageReq - current message of request.
void methodConnect(HTTP_REQUEST *currentReq, int deskription)
{
    WRITE_LOG("Start | status: %d | currentReq: %p | deskription: %d\n", currentReq->status, currentReq, deskription);
    if(isUseInLog("Method:"))
    {
        getLogeW3CLog()->Method = malloc(strlen("CONNECT") + 1);
        sprintf(getLogeW3CLog()->Method,"CONNECT");
    }
    currentReq->status = HTTP_501_NOT_IMPLEMENTED;
    currentReq->lenResponse = LENGTH_ISNT_IMPLEMENTED_RESPONSE;
    currentReq->body = malloc(currentReq->lenResponse * sizeof(char));
    RESPONSE_ERROR(currentReq->body, currentReq->version, currentReq->status);
    
    if(sendAll(deskription, currentReq->body, &currentReq->lenResponse) == ERROR_RUN)
    {
        WRITE_LOG("Error. sendAll is failed%s \n", " ");
    }
    freeNew(&currentReq->body);
    return;
}
//purpose: apply partial modifications to the resource.
//parameter:
//    currentReq - object for current message of request.
//    messageReq - current message of request.
void methodPatch(HTTP_REQUEST *currentReq, int deskription)
{
    WRITE_LOG("Start | status: %d | currentReq: %p | deskription: %d\n", currentReq->status, currentReq, deskription);
    if(isUseInLog("Method:"))
    {
        getLogeW3CLog()->Method = malloc(strlen("PATCH") + 1);
        sprintf(getLogeW3CLog()->Method,"PATCH");
    }
    currentReq->status = HTTP_501_NOT_IMPLEMENTED;
    currentReq->lenResponse = LENGTH_ISNT_IMPLEMENTED_RESPONSE;
    currentReq->body = malloc(currentReq->lenResponse * sizeof(char));
    RESPONSE_ERROR(currentReq->body, currentReq->version, currentReq->status);
    
    if(sendAll(deskription, currentReq->body, &currentReq->lenResponse) == ERROR_RUN)
    {
        WRITE_LOG("Error. sendAll is failed%s \n", " ");
    }
    freeNew(&currentReq->body);
    return;
}
//purpose: apply partial modifications to the resource.
//parameter:
//    currentReq - object for current message of request.
//    messageReq - current message of request.
void methodUnknow(HTTP_REQUEST *currentReq, int deskription)
{
    WRITE_LOG("Start | status: %d | currentReq: %p | deskription: %d\n", currentReq->status, currentReq, deskription);
    if(isUseInLog("Method:"))
    {
        getLogeW3CLog()->Method = malloc(strlen("UNKNOW") + 1);
        sprintf(getLogeW3CLog()->Method,"UNKNOW");
    }
    
    currentReq->status = HTTP_400_BAD_REQUEST;
    currentReq->lenResponse = LENGTH_ISNT_IMPLEMENTED_RESPONSE;
    currentReq->body = malloc(currentReq->lenResponse * sizeof(char));
    RESPONSE_ERROR(currentReq->body, 0, currentReq->status);
    
    if(sendAll(deskription, currentReq->body, &currentReq->lenResponse) == ERROR_RUN)
    {
        WRITE_LOG("Error. sendAll is failed%s \n", " ");
    }
    freeNew(&currentReq->body);
   
    
    /*currentReq->lenResponse = LENGTH_BAD_REQUEST_RESPONSE;
    if(sendAll(deskription, "400 BAD REQUEST", &currentReq->lenResponse) == ERROR_RUN)
    {
        WRITE_LOG("Error. sendAll is failed%s \n", " ");                
    }*/
    WRITE_LOG("End | status: %d\n", deskription);
    return;
}
