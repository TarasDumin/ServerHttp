#ifndef _AUXILIARY_FUNCTION_H_
#define _AUXILIARY_FUNCTION_H_

#define LENGTH_STRING_TIME  (30)
#define MAX_LENGTH_TEXT     (256)
#define STDIN               (0)  // file descriptor for standard input
#define LENGTH_EXIT         (4)

char* getTimeS(void);
void convertToLower(char*, char*);
void freeNew(char **member);
int doesServerHaveToWorkCommand(void);
int doesServerHaveToWorkConfig(void);
int checkExit(void);
void sig_handler(int);

#endif //_AUXILIARY_FUNCTION_H_
