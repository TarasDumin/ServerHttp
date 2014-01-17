#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <net/if.h>
#include <errno.h>
#include <ifaddrs.h>
#include <time.h>
#include "workWithSockets.h" 
#include "debugLogging.h"
#include "auxiliaryFunction.h"
#include "accessLoggin.h"

struct sockaddr_in socketADD;

//purpose: is freed all memory which was used freeObject
//parameter:
//     IPadd: address that is verified
//return: true or false
int checkExistenceSpecifiedAddr(char *IPadd)
{
    struct ifaddrs *interfaceArray = NULL, *tempIfAddr = NULL;
    void *tempAddrPtr = NULL;
    char addressOutputBuffer[INET6_ADDRSTRLEN];
    int result;
 
    WRITE_LOG("Start | IPadd: %p\n", IPadd);

    if (!getifaddrs(&interfaceArray))// retrieve the current interfaces
    {    
        for(tempIfAddr = interfaceArray; tempIfAddr != NULL; tempIfAddr = tempIfAddr->ifa_next)
        {
            if(tempIfAddr->ifa_addr->sa_family == AF_INET)
            {
                tempAddrPtr = &((struct sockaddr_in *)tempIfAddr->ifa_addr)->sin_addr;
            }
            else
            {
                tempAddrPtr = &((struct sockaddr_in6 *)tempIfAddr->ifa_addr)->sin6_addr;
            }
            if(inet_ntop(tempIfAddr->ifa_addr->sa_family, tempAddrPtr, addressOutputBuffer, sizeof(addressOutputBuffer)))
            {
                if(!memcmp(addressOutputBuffer, IPadd, strlen(addressOutputBuffer)))
                {
                     result = SUCCESS;
                     break;
                }
            }
            result = NOT_EXIST;
        }

        freeifaddrs(interfaceArray);             // free the dynamic memory
        interfaceArray = NULL;                   // prevent use after free
    }
    else
    {
        WRITE_LOG("Error.  getifaddrs was failed%s\n", "");
        result = ERROR_RUN;
    }

    WRITE_LOG("End | result: %d\n", result);
    return result;
}

//purpose: accept to client with log date about client
//return: socket descriptor of server
int connectToSocket(void)
{
    int yes = 1; 
    int deskription = 0;
    int result;
    int port = BROWSER_PORT;
    int maxConnect = MAX_CONNECTION;
    char IPaddress[INET6_ADDRSTRLEN];

    WRITE_LOG("Start | %s\n", "");

    getValueForKeyNumber("config.txt", "Port to listen:", &port);
   
    deskription = socket(PF_INET, SOCK_STREAM, 0);
    memset(&socketADD, 0, sizeof(socketADD));
    socketADD.sin_family = AF_INET;

    if(SUCCESS == getValueForKeyString("config.txt", "Is used IP address:", IPaddress) && SUCCESS == checkExistenceSpecifiedAddr(IPaddress))
    {
        socketADD.sin_addr.s_addr = inet_addr(IPaddress);
    }
    else
    {
        socketADD.sin_addr.s_addr = INADDR_ANY;
    }
    
    socketADD.sin_port = htons((unsigned short)port);
    
    if (setsockopt(deskription, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == ERROR_RUN) 
    {
        result = ERROR_RUN;
        WRITE_LOG("Error. Setsockopt was failing. | result: %d\n", result);
        return result;
    } 

    if(bind(deskription,(struct sockaddr*)&socketADD, sizeof(socketADD)) == ERROR_RUN)
    {
        result = ERROR_RUN;
        WRITE_LOG("Error. Bind was failing. | result: %d\n", result);
        return result;
    }
    getValueForKeyNumber("config.txt", "Max number of connect:", &maxConnect);

    
    if(listen(deskription, maxConnect) == ERROR_RUN)
    {
        result = ERROR_RUN;
        WRITE_LOG("Error. listen was failing. | result: %d\n", result);    
        return result;
    }
    result = deskription;
    WRITE_LOG("End | Server is working now. | deskription: %d\n", result);
    return result;
}

//purpose: accept to client with log date about client
//parameters:
//   deskript - socket descriptor
//return: socket descriptor of client
int acceptWithLogger(int deskription)
{
    struct sockaddr_in socketClient;
    socklen_t clientLen = sizeof(socketClient);
    int deskriptionClient;
    int result;
    fd_set fds;
    struct timeval tv;
    WRITE_LOG("Start | Deskription: %d\n", deskription);
    
    FD_ZERO(&fds);
    FD_SET(deskription, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = TIME_WAIT_ACCEPT;
    tv.tv_usec = 0;

    // wait until timeout or data received
    result = select(deskription + 1, &fds, NULL, NULL, &tv);

    if (result == 0)
    {
        result = TIME_OUT; // timeout!
    }
    else if (result == ERROR_RUN)
    {
        WRITE_LOG("Error. select was failed: %s\n", "");
        result = ERROR_RUN; // error
    }
    else
    {     
        deskriptionClient = accept(deskription, (struct sockaddr *)&socketClient, &clientLen);
        if(isUseInLog("Server IP address:"))
        {
            struct sockaddr_in socketServer;
            socklen_t serverLen = sizeof(socketServer);
            char *IPaddr = NULL;
            int lenAddr = 0;
        
            getsockname(deskriptionClient, (struct sockaddr*)&socketServer, &serverLen);
            
            IPaddr = inet_ntoa(socketServer.sin_addr);
            lenAddr = strlen(IPaddr);        

            getLogeW3CLog()->ServerIPAddress = malloc(lenAddr + 1);
            sprintf(getLogeW3CLog()->ServerIPAddress,"%s", IPaddr);
        }
        
        if (deskriptionClient < 0) 
        {
            WRITE_LOG("Error. DeskriptionClient wasn't opened: %s\n", "");
            result = ERROR_RUN;
        }
        else
        {
            result = deskriptionClient;
        }
    }
    if(isUseInLog("Client IP address:"))
    {
        char *IPaddr = inet_ntoa(socketClient.sin_addr);
        int lenAddr = strlen(IPaddr);
        
        getLogeW3CLog()->ClientIPAddress =inet_ntoa(socketClient.sin_addr);
        getLogeW3CLog()->ClientIPAddress = malloc(lenAddr + 1);
        sprintf(getLogeW3CLog()->ClientIPAddress,"%s", IPaddr);
    }
    
    WRITE_LOG("End | Deskription: %d | IP address is: %s | port is: %d | result: %d\n", deskription, inet_ntoa(socketClient.sin_addr), (int) ntohs(socketClient.sin_port), result);    
    return result;
}
//purpose: read from socket with the specified waiting time for data
//parameters:
//   deskript - socket descriptor
//   *buf - a place to read symbols
//   len - how many is read symbols
//   timeout - time of keap-alive
//return: count symbols was read
int readWithTimeOut(int deskript, char *buf, int len, int timeout)
{
    fd_set fds;
    int result;
    struct timeval tv;

    WRITE_LOG("Start | deskript: %d | pBuffer: %p | len: %d | time wait: %d\n", deskript, buf, len, timeout);
    
    // set up the file descriptor set
    FD_ZERO(&fds);
    FD_SET(deskript, &fds);

    // set up the struct timeval for the timeout
    tv.tv_sec = timeout;
    tv.tv_usec = 0;

    // wait until timeout or data received
    result = select(deskript + 1, &fds, NULL, NULL, &tv);
    if (result == 0)
    {
        result = TIME_OUT; // timeout!
    }
    else if (result == ERROR_RUN)
    {
        result = ERROR_RUN; // error
    }
    else
    {
        result = read(deskript, buf, len);
        buf[result] = 0;
    }

    WRITE_LOG("End | deskript: %d | pBuffer: %p | len: %d | time wait: %d |result: %d\n", deskript, buf, len, timeout, result);    
    return result;
}
//purpose: send all symbols
//parameters:
//   deskript - socket descriptor
//   *buf - a place to read symbols
//   len - how many is read symbols
//return: count symbols was sent
int sendAll(int deskript, char *buf, int *len)
{
    int total = 0;        // how many bytes we've sent
    int number = 0;  
    int result;  

    WRITE_LOG("Start | deskript: %d | pBuffer: %p | len: %d\n", deskript, buf, *len);

    while(total < *len) 
    {
        number = send(deskript, buf+total, *len - total, 0);
        if (number == ERROR_RUN) 
        { 
            break; 
        }
        total += number;
    }
    *len = total; // return number actually sent here
    result = (number == ERROR_RUN)? ERROR_RUN : 0;

    WRITE_LOG("End | deskript: %d | pBuffer: %p | len: %d | result: %d\n", deskript, buf, *len, result);
    return result; // return -1 on failure, 0 on success
} 
