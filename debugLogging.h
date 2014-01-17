#ifndef _DEBUG_LOGGING_H_
#define _DEBUG_LOGGING_H_

#define DEBUG_MODE
#ifdef  DEBUG_MODE
    #define WRITE_LOG(text,...)             \
    {                                       \
        char buffer[256];                   \
        sprintf(buffer, text, __VA_ARGS__);             \
        writeLog(__FILE__, __func__,  __LINE__, buffer);\
    }
#else
    #define WRITE_LOG(text,...)             \
    if(text[0] == 'E' &&                    \
       text[1] == 'r' &&                    \
       text[2] == 'r' &&                    \
       text[3] == 'o' &&                    \
       text[4] == 'r')                      \
    {                                       \
        char buffer[256];                   \
        sprintf(buffer, text, __VA_ARGS__);             \
        writeLog(__FILE__, __func__,  __LINE__, buffer);\
    }
#endif

void startDebug();
void writeLog(const char*,const char*, int, char*);
void endDebug();

#endif //_DEBUG_LOGGING_H_
