#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include "request.h"
#include "methods.h"
#include "debugLogging.h"
#include "auxiliaryFunction.h"
#include "accessLoggin.h"


static void getMethodHTTP1_0(HTTP_REQUEST *currentReq, char *messageReq);
static void getMethodHTTP1_1(HTTP_REQUEST *currentReq, char *messageReq);
//purpose: delete request
//paremeter:
//     currentReq - request that need is deleted
void deleteRequest(HTTP_REQUEST* currentReq)
{
    WRITE_LOG("Start | currentReq: %p\n", currentReq);

    freeNew(&currentReq->body);
    freeNew(&currentReq->URI);
    freeNew(&currentReq->header);
    free(currentReq);

    WRITE_LOG("End | currentReq: %p\n", currentReq);
    return;
}
//purpose: get request object which use for save data about request
//return: request object
HTTP_REQUEST *createRequestObject(void)
{
    HTTP_REQUEST *requestObject = malloc(sizeof(HTTP_REQUEST));
    WRITE_LOG("Start | point: %p\n", requestObject);

    requestObject->version = UNDEFINED_HTTP;
    requestObject->status = HTTP_200_OK;
    requestObject->isKeapAliveUsing = NO_Q;
    requestObject->isChunkUsing = NO_Q;
    if(getValueForKeyNumber("config.txt", "keep-alive:", &requestObject->timeOfKeapAlive) != SUCCESS)
    {    
        requestObject->timeOfKeapAlive = DEFAULT_TIME_KEAP_ALIVE;
    }

    requestObject->lenResponse = 0;
    requestObject->lenURI = 0;
    requestObject->lenBodyResponse = 0;
 
    requestObject->header = NULL;
    requestObject->body = NULL;
    requestObject->URI = NULL;

    requestObject->getMethodHTTP = getMethodHTTP1_0;
    requestObject->MethodHTTP = methodUnknow;
    WRITE_LOG("End | point: %p\n", requestObject);
    return requestObject;
}
//purpose: get method from request
//parameter:
//    currentReq - object for current message of request.
//    messageReq - current message of request.
void getVersion(HTTP_REQUEST *currentReq, char *messageReq)
{
    char *pBeginThirdWord;
    char currentVersion[MAX_LENGTH_VERSION];
    int lenVersion = 0;

    WRITE_LOG("Start | status: %d : message: %p\n", currentReq->status, messageReq);

    pBeginThirdWord = strstr(strstr(messageReq," ") + 1, " ") + 1;
    if(pBeginThirdWord)
    {
       lenVersion = strlen(pBeginThirdWord);
    }
    else
    {
        strcpy(currentVersion, "Error");
    }

    if(MAX_LENGTH_VERSION > lenVersion &&
                        0 < lenVersion)
    {
        strncpy(currentVersion, pBeginThirdWord, lenVersion);
        currentVersion[lenVersion] = 0;
        if(!strcmp("HTTP/1.0",currentVersion))
        {
            currentReq->version = HTTP_1_0;
            currentReq->isKeapAliveUsing = NO_Q;
            currentReq->isChunkUsing = NO_Q;
            currentReq->getMethodHTTP = getMethodHTTP1_0;
        }
        else if(!strcmp("HTTP/1.1",currentVersion))
        {
           currentReq->version = HTTP_1_1;
           currentReq->isKeapAliveUsing = YES_Q;
           currentReq->isChunkUsing = YES_Q;
           currentReq->getMethodHTTP = getMethodHTTP1_1;
        }
        else if(!strncmp("HTTP/", currentVersion, strlen("HTTP/")))
        {
            currentReq->status  = HTTP_505_HTTP_VERSION_NOT_SUPPORTED;
            currentReq->version = UNDEFINED_HTTP;
        }
        else
        {
            currentReq->status  = HTTP_400_BAD_REQUEST;
            currentReq->version = UNDEFINED_HTTP;
        }
    }
    else
    {
        currentReq->status  = HTTP_400_BAD_REQUEST;  //to do: check answer
        currentReq->version = UNDEFINED_HTTP;
    }
    WRITE_LOG("End | status: %d | version HTTP/1.%d\n", currentReq->status, currentReq->version);
    return;
}

//purpose: get method from request.
//parameter:
//    currentReq - object for current message of request.
//    messageReq - current message of request.
void getMethodHTTP1_0(HTTP_REQUEST *currentReq, char *messageReq)
{
    char *pBegin;
    char *pFirstSpace;
    char currentMethod[MAX_LENGTH_METHOD];
    int lenMethod = 0;
 
    WRITE_LOG("Start | status: %d : message: %p\n", currentReq->status, messageReq);
 
    pFirstSpace = strstr(messageReq," ");
    pBegin = messageReq;
    if(pFirstSpace &&
       pBegin)
    {
        lenMethod = pFirstSpace - pBegin;
    }
    else
    {
        strcpy(currentMethod, "Error");
    }
 
    if(MAX_LENGTH_METHOD > lenMethod &&
                       0 < lenMethod)
    { 
         strncpy(currentMethod, pBegin, lenMethod);
         currentMethod[lenMethod] = 0;
         if(!strcmp("GET",currentMethod))
         {

             currentReq->method = HTTP_GET;
             currentReq->MethodHTTP = methodGet;
         }
         else if(!strcmp("HEAD",currentMethod))
         {
             currentReq->method = HTTP_HEAD;
             currentReq->MethodHTTP = methodHead;
         }
         else if(!strcmp("POST",currentMethod))
         {
             currentReq->method = HTTP_POST;
             currentReq->MethodHTTP = methodPost;
         }
         else 
         { 
             currentReq->status  = HTTP_501_NOT_IMPLEMENTED;  //to do: check answer
             currentReq->method = HTTP_EXTENSION_METHOD;     
             currentReq->MethodHTTP = methodUnknow;
         }
    }
    else
    {
         currentReq->status  = HTTP_400_BAD_REQUEST;  //to do: check answer
    }
    WRITE_LOG("End | status: %d | method %s\n", currentReq->status, currentMethod);
    return;
}
//purpose: get method from request.
//parameter:
//    currentReq - object for current message of request.
//    messageReq - current message of request.
void getMethodHTTP1_1(HTTP_REQUEST *currentReq, char *messageReq)
{
   char *pBegin;
   char *pFirstSpace;
   char currentMethod[MAX_LENGTH_METHOD];
   int lenMethod = 0;

   WRITE_LOG("Start | status: %d : message: %p\n", currentReq->status, messageReq);

   pFirstSpace = strstr(messageReq," ");
   pBegin = messageReq;
   if(pFirstSpace &&
      pBegin)
   {
       lenMethod = pFirstSpace - pBegin;
   }
   else
   {
       strcpy(currentMethod, "Error");
   }

   if(MAX_LENGTH_METHOD > lenMethod &&
                      0 < lenMethod)
   {
       strncpy(currentMethod, pBegin,lenMethod);
       currentMethod[lenMethod] = 0;
       if(!strcmp("GET",currentMethod))
       {
           currentReq->method = HTTP_GET;
           currentReq->MethodHTTP = methodGet;
       }
       else if(!strcmp("HEAD", currentMethod))
       {
           currentReq->method = HTTP_HEAD;
           currentReq->MethodHTTP = methodHead;
       }
       else if(!strcmp("POST", currentMethod))
       {
           currentReq->method = HTTP_POST;
           currentReq->MethodHTTP = methodPost;
       }
       else if(!strcmp("PUT", currentMethod))
       {
           currentReq->method = HTTP_PUT;
           currentReq->MethodHTTP = methodPut;
       }
       else if(!strcmp("DELETE", currentMethod))
       {
           currentReq->method = HTTP_DELETE;
           currentReq->MethodHTTP = methodDelete;
       }
       else if(!strcmp("OPTIONS", currentMethod))
       {
           currentReq->method = HTTP_OPTIONS;
           currentReq->MethodHTTP = methodOptions;
       }
       else if(!strcmp("TRACE", currentMethod))
       {
           currentReq->method = HTTP_TRACE;
           currentReq->MethodHTTP = methodTrace;
       }
       else if(!strcmp("CONNECT", currentMethod))
       {
           currentReq->method = HTTP_CONNECT;
           currentReq->MethodHTTP = methodConnect;
       }
       else if(!strcmp("PATCH", currentMethod))
       {
           currentReq->method = HTTP_PATCH;
           currentReq->MethodHTTP = methodPatch;
       }
       else 
       {
           currentReq->status  = HTTP_501_NOT_IMPLEMENTED; //to do: check answer
           currentReq->method = HTTP_EXTENSION_METHOD;
           currentReq->MethodHTTP = methodUnknow;
       }
   }
   else
   {
       currentReq->status  = HTTP_400_BAD_REQUEST; //to do: check answer
   }
   WRITE_LOG("End | status: %d | method %s\n", currentReq->status, currentMethod);
   return;
}
//purpose: get value using key
//parameter:
//    name - name of file
//    Key - key for search.
//    value - value.
//return: status of execution
int getValueForKeyNumber(char *name, char *Key, int *Value)
{
    char* configData;
    char* pStartValue;
    int sizeFile = 0, result = SUCCESS;
    FILE *File;
    
    WRITE_LOG("Start | Key: %s | Value: %p\n", Key, Value);

    File = fopen(name, "r");
    if (File) 
    {   
        fseek(File, 0L, SEEK_END);
        sizeFile = ftell(File);
        fseek(File, 0L, SEEK_SET);

        configData = malloc(sizeFile * sizeof(char));
        fread(configData, sizeFile, sizeof(char), File);
        fclose(File);
          

        pStartValue = strstr(configData, Key) + strlen(Key);
        if(pStartValue)
        {
            *Value = atoi(pStartValue);
            if(!*Value)
            {
                WRITE_LOG("Error. Bad value in config %s\n", " ");
                result = KEY_ISNT_FOUND;
            }
        }
        else
        {
            WRITE_LOG("Error. Didn't find key %s\n", " ");
            result = KEY_ISNT_FOUND;
        }
        freeNew(&configData);
    }else{
        WRITE_LOG("Error. File config.txt isn't found%s\n", " ");
        result = ERROR_RUN;
    }

    WRITE_LOG("End | Key: %s | Value: %p | result: %d\n", Key, Value, result);
    return result;
}
//purpose: get value using key
//parameter:
//    Key - key for search.
//    value - value.
//return: status of execution
int getValueForKeyString(char *name, char *Key, char *Value)
{
    char* pStartValue;
    char* pEndString;
    int  result = 0;
    FILE *File;

    WRITE_LOG("Start | Key: %s | Value: %p\n", Key, Value);

    File = fopen(name, "r");
    if (File) 
    {   
        char line[MAX_LENGTH_STRING], *pFile;
        do
        {
            pFile = fgets(line, sizeof(line), File);
            if(!memcmp(line, Key, strlen(Key)))
            {
                
                pStartValue = line + strlen(Key);
                while(*pStartValue == ' ')
                {
                    pStartValue++;
                }
                pEndString = pStartValue + strlen(pStartValue);
                while(*(pEndString - 1) == ' ' ||
                      *(pEndString - 1) == '\r' ||
                      *(pEndString - 1) == '\n')
                {
                    pEndString--;
                }
                strncpy(Value, pStartValue, pEndString - pStartValue);
                Value[pEndString - pStartValue] = 0;
                result = SUCCESS;
                break;
            }
            result = KEY_ISNT_FOUND;
        }while(pFile);
        fclose(File);
    }else{
        WRITE_LOG("Error. File config.txt isn't found%s\n", " ");
        result = ERROR_RUN;
    }

    WRITE_LOG("End | Key: %s | Value: %p | result: %d\n", Key, Value, result);
    return result;
}
//purpose: get URI from request
//parameter:
//    currentReq - info about request.
//    messageReq - current request.
void getURI(HTTP_REQUEST *currentReq, char *messageReq)
{
    char *pSecondWord;
    char *pSecondSpace;
    char defaultFolder[MAX_LENGTH_STRING];
    char document[MAX_LENGTH_STRING];
    int result = 0;
   
    WRITE_LOG("Start | status: %d : message: %p\n", currentReq->status, messageReq);

    result = getValueForKeyString("config.txt", "Root folder:", defaultFolder);
    if(ERROR_RUN == result)
    {
        currentReq->status = HTTP_500_INTERNAL_SERVER_ERROR;
        WRITE_LOG("End | incorrect default folder | result: %d\n", result);
        return;
    }

    pSecondWord = strstr(messageReq," ") + 1;
    pSecondSpace = strstr(pSecondWord," ");
    currentReq->lenURI = pSecondSpace - pSecondWord;
    //<схема>://<логин>:<пароль>@<хост>:<порт>/<URL‐путь>?<параметры>#<якорь>
    if(*pSecondWord == '/' &&
       currentReq->lenURI < MAX_LENGTH_STRING - 1)
    {
        char *pParameters = NULL;
        if(currentReq->lenURI == 1)
        {
            result = getValueForKeyString("config.txt", "Default document:", document);
            strcat(defaultFolder, "/");
            if(ERROR_RUN == result)
            {
                currentReq->status = HTTP_500_INTERNAL_SERVER_ERROR;
                WRITE_LOG("End | Error. incorrect default file | result: %d\n", result);
                return;
            }
        }
        else
        {
            pParameters = strchr(pSecondWord, '?');
            if(pParameters)
            {
                currentReq->lenURI = pParameters - pSecondWord;
            }
            strncpy(document, pSecondWord, currentReq->lenURI);
            document[currentReq->lenURI] = 0;
        }
        currentReq->lenURI = strlen(document) + strlen(defaultFolder) + 1;
        currentReq->URI = malloc(currentReq->lenURI);
        strcpy(currentReq->URI, defaultFolder);
        strcat(currentReq->URI, document);

        if(isUseInLog("URIStem:"))
        {
            getLogeW3CLog()->URIStem = malloc(currentReq->lenURI);
            strcpy(getLogeW3CLog()->URIStem, currentReq->URI);
        }
    }
    else
    {
        if(currentReq->lenURI >= MAX_LENGTH_STRING - 1)
        {
            currentReq->status = HTTP_414_REQUEST_URI_TOO_LONG;
        }
        else
        {
            currentReq->status = HTTP_400_BAD_REQUEST;
        }
    } 

    WRITE_LOG("End | status: %d \n", currentReq->status);
    return;
}
//purpose: parse value when parameter: connection
//parameter:
//    currentReq - object for current message of request.
//    value - current value.
//    lenValue - lenght value.
#define LENGTH_TIMEOUT           (8)
void ParseConnection(HTTP_REQUEST *currentReq, char* value, int lenValue)
{
    if(!memcmp(value, "keep-alive", lenValue))
    {
        currentReq->isKeapAliveUsing = YES_Q;
    }
    else if(!memcmp(value, "close", lenValue))
    {
        currentReq->isKeapAliveUsing = NO_Q;
    }
}
//purpose: parse value when parameter: Transfer-Encoding
//parameter:
//    currentReq - object for current message of request.
//    value - current value.
//    lenValue - lenght value.
void ParseTransferEncoding(HTTP_REQUEST *currentReq, char* value, int lenValue)
{
    if(!memcmp(value, "chunked", lenValue))
    {
        currentReq->isChunkUsing = YES_Q;
    }
}
//purpose: parse value when parameter: Keep-Alive
//parameter:
//    currentReq - object for current message of request.
//    value - current value.
//    lenValue - lenght value.
void ParseKeepAlive(HTTP_REQUEST *currentReq, char* value, int lenValue)
{
    if(!memcmp(value, "timeout=", LENGTH_TIMEOUT))
    {
        value += LENGTH_TIMEOUT;
        currentReq->timeOfKeapAlive = atoi(value);
    }
}
//purpose: parse value when parameter: Accept-Language
//parameter:
//    currentReq - object for current message of request.
//    value - current value.
//    lenValue - lenght value.
void ParseAcceptLanguage(HTTP_REQUEST *currentReq, char* value, int lenValue)
{
    //to do in future
}
//purpose: parse value when parameter: Referer
//parameter:
//    currentReq - object for current message of request.
//    value - current value.
//    lenValue - lenght value.
void ParseReferer(HTTP_REQUEST *currentReq, char* value, int lenValue)
{
    
}
//purpose: parse value when parameter: Accept-Language
//parameter:
//    currentReq - object for current message of request.
//    ParseValue - function for parse value.
//    value - current value.
void IterateOverAllValue(HTTP_REQUEST *currentReq, PARSE_VALUE ParseValue, char* value)
{
    char *startValue = value;
    char *endValue = NULL;

    WRITE_LOG("Start | status: %d | pValue %s\n", currentReq->status, value);
    do
    {
        while(*startValue == ';')
        {
            startValue++;
        }
        endValue = strstr(startValue,";");
        if(endValue)
        {
            *endValue = '\0';
            ParseValue(currentReq, startValue, endValue - startValue);
            *endValue = ';';
        }
        else
        {
            ParseValue(currentReq, startValue, strlen(startValue));
        }
        startValue = endValue + 1;
    }while(endValue);
}
//purpose: parse a string of the request
//parameter:
//    currentReq - info about request.
//    parameter - current parameter.
//    value - current value.
#define LENGTH_TRANSFER_ENCODING (18)
#define LENGTH_KEEP_ALIVE        (11)
#define LENGTH_CONNECTION        (11) 
#define LENGTH_ACCEPT_LANGUAGE   (16)
#define LENGTH_REFERER           (8)
static void parseOneParameter(HTTP_REQUEST *currentReq, char *parameter, char* value)
{
    WRITE_LOG("Start | status: %d | parameter: %s | pValue %s\n", currentReq->status, parameter, value);     

    if(!memcmp(parameter, "Connection:", LENGTH_CONNECTION))
    {
        IterateOverAllValue(currentReq, ParseConnection, value);
    }
    if(!memcmp(parameter, "Transfer-Encoding:", LENGTH_TRANSFER_ENCODING))
    {
        IterateOverAllValue(currentReq, ParseTransferEncoding, value);
    }
    if(!memcmp(parameter, "keep-alive:", LENGTH_KEEP_ALIVE))
    {
        IterateOverAllValue(currentReq, ParseKeepAlive, value); 
    }
    if(!memcmp(parameter, "Accept-Language:", LENGTH_ACCEPT_LANGUAGE))
    {
       IterateOverAllValue(currentReq, ParseAcceptLanguage, value); 
    }
    if(!memcmp(parameter, "Referer:", LENGTH_REFERER))
    {
        ParseReferer(currentReq, value, strlen(value));
    }
    WRITE_LOG("End | status: %d : parameter: %s | pValue %s\n", currentReq->status, parameter, value);
    return;
}
#define MIDDLE_REQUEST 0
//purpose: split parameters of request and processing them
//parameter:
//    currentReq - info about request.
//    buff - request.
//    count - length of the request(length buff).
static int requestParametersParse(HTTP_REQUEST *currentReq, const char* buff, int count)
{
    static char oneParameterBuffer[MAX_LENGTH_STRING];
    static unsigned int ParseBuff_ptr = 0;
    static unsigned int lenName = 0;

    WRITE_LOG("Start | status: %d : buff: %p | count %d\n", currentReq->status, buff, count);
    while(count--)
    {
        if('\r' == *buff)
        {
            if('\n' == *(buff + 1))
            {
                char *parameter;
                char *value;
                int lenValue = ParseBuff_ptr - lenName;
                if(ParseBuff_ptr == 0)
                {
                    return END_REQUEST;
                }
                
                if(lenName && 
                   lenValue)
                {
                     parameter = malloc(lenName + 1);
                     strncpy(parameter, oneParameterBuffer, lenName);
                     parameter[lenName] = 0;
                     value = malloc(lenValue + 1);
                     strncpy(value, oneParameterBuffer + lenName, lenValue);
                     value[lenValue] = 0;
                     parseOneParameter(currentReq, parameter, value);
                     free(parameter);
                     free(value);    
                }
                lenName = 0;
                ParseBuff_ptr = 0;
                count--;
                buff++;
            }
        }
        else if(' ' != *buff) 
        {
            oneParameterBuffer[ParseBuff_ptr++] = *buff;
            if( MAX_LENGTH_STRING <= ParseBuff_ptr )
            {
                ParseBuff_ptr = MAX_LENGTH_STRING - 1;
            }
            if(':' == *buff &&
                !lenName)
            {
                lenName = ParseBuff_ptr;
            }
        }   
        buff++;
    }
    WRITE_LOG("End | status: %d : buff: %p | count %d\n", currentReq->status, buff, count);
    return MIDDLE_REQUEST;
}
#define LENGTH_SYMBOL_NEWLINE 2
char* proccessFirstString(HTTP_REQUEST *currentReq, char *messageReq)
{
   WRITE_LOG("Start | status: %d : message: %p\n", currentReq->status, messageReq);
   if(currentReq->version == UNDEFINED_HTTP)
   {
       char firstString[MAX_LENGTH_FIRST_STRING];
       char *endString;
       int lenFirstString = 0;
       endString = strstr(messageReq, "\r\n");
       
       if(endString)
       {
           lenFirstString = endString - messageReq;
       }
       else
       {
           lenFirstString = strlen(messageReq);
       }
    
       if(0 < lenFirstString &&
          MAX_LENGTH_FIRST_STRING > lenFirstString)
       {
           strncpy(firstString, messageReq, lenFirstString);
           messageReq = endString + LENGTH_SYMBOL_NEWLINE;
           firstString[lenFirstString] = 0;

           getVersion(currentReq, firstString);
           if(HTTP_200_OK == currentReq->status)
           {
               currentReq->getMethodHTTP(currentReq, firstString);
           }
           if(HTTP_200_OK == currentReq->status)
           {
               getURI(currentReq, firstString);
           }
       }
       else
       {
           currentReq->status = HTTP_400_BAD_REQUEST;
       }
   }
   WRITE_LOG("End | status: %d \n", currentReq->status);
   return messageReq;
}
#define EXIT_READ 1
//purpose: is checking request from client and is writing parameters in appropriate variables
//    
int parseParameters(HTTP_REQUEST *currentReq, char *messageReq, int len)
{
   char *pMessage = messageReq;
   int result = 0;
   
   WRITE_LOG("Start | currentReq: %p | message: %p | len: %d | status: %d\n", currentReq, messageReq, len, currentReq->status);
  
   pMessage = proccessFirstString(currentReq, pMessage);
   if(HTTP_200_OK == currentReq->status)
   {
       result = requestParametersParse(currentReq, pMessage, strlen(pMessage));
   }
   
   if(HTTP_200_OK != currentReq->status)
   {
       result = EXIT_READ;
   }
   
   WRITE_LOG("End | result: %d | status: %d\n", result, currentReq->status);
   return result;
}
