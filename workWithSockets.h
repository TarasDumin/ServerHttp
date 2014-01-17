#ifndef _WORK_WITH_SOCKETS_H_
#define _WORK_WITH_SOCKETS_H_

#define BROWSER_PORT       (80)
#define MAX_CONNECTION     (5)
#define MAX_LEN_IP_ADDRESS (16)
#define TIME_OUT           (-2)
#define NOT_EXIST          (0)
#define TIME_WAIT_ACCEPT   (5)


int connectToSocket();
int readWithTimeOut(int , char*, int, int);
int sendAll(int ,char*, int*);
int acceptWithLogger(int);

#endif //_WORK_WITH_SOCKETS_H_

