#ifndef _ACCESS_LOG_H_
#define _ACCESS_LOG_H_

#include "request.h"

#define TRUE                    (1)
#define FALSE                   (0)
#define LEN_SERVER_NAME         (8)
#define MAX_LENGTH_STRING       (256)

typedef struct _W3C_LOG_
{
    char *Date;                         //The date on which the activity occurred.
    char *Time;                         //The time, in coordinated universal time (UTC), at which the activity occurred.
    char *ClientIPAddress;              //The IP address of the client that made the request.
    char *UserName;                     //The name of the authenticated user who accessed your server. Anonymous users are indicated by a hyphen.
    char *ServiceNameAndInstanceNumber;	//The Internet service name and instance number that was running on the client.	
    char *ServerName;                   //The name of the server on which the log file entry was generated.
    char *ServerIPAddress;              //The IP address of the server on which the log file entry was generated.
    char *ServerPort;                   //The server port number that is configured for the service.
    char *Method;                       //The requested action, for example, a GET method.
    char *URIStem;                      //The target of the action, for example, Default.htm.
    char *URIQuery;                     //The query, if any, that the client was trying to perform. A Universal Resource Identifier (URI) query is necessary only for dynamic pages.
    int HTTPStatus;                     //The HTTP status code.
    char *Win32Status;                  //The Windows status code.
    int BytesSent;                      //The number of bytes that the server sent.
    char *BytesReceived;                //The number of bytes that the server received.
    char *TimeTaken;                    //The length of time that the action took, in milliseconds.
    char *ProtocolVersion;              //The protocol version —HTTP or FTP —that the client used.
    char *Host;                         //The host header name, if any.
    char *UserAgent;                    //The browser type that the client used.
    char *Cookie;                       //The content of the cookie sent or received, if any.
    char *Referrer;                     //The site that the user last visited. This site provided a link to the current site.
    char *ProtocolSubstatus;            //The substatus error code.
}W3C_LOG;

void initW3CLog(void);
W3C_LOG *getLogeW3CLog(void);
void writeInLogFile(void);
YES_NO_QUESTION isUseInLog(char *Key);
void clearW3CLog(void);
int isEmptyLogDate(char*);

#endif //_ACCESS_LOG_H_
