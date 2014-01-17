#ifndef _METHODS_H_
#define _METHODS_H_
#include "request.h"
#define LENGTH_PART_CHUNK                (16)
#define PLACE_FOR_END_CHUNK              (2)
#define LENGTH_ISNT_IMPLEMENTED_RESPONSE (71)
#define LENGTH_BAD_REQUEST_RESPONSE      (17)
#define LENGTH_CHUNK                     (25600)
#define READ_FULL_FILE                   (-1)
#define SIZE_CHUNK_WITHOUT_DATE          (5)
#define MAX_LENGTH_TYPE                  (10)
#define MAX_LENGTH_CONTENT_TYPE          (40)

void methodGet(HTTP_REQUEST*, int);
void methodHead(HTTP_REQUEST*, int);
void methodPost(HTTP_REQUEST*, int);
void methodPut(HTTP_REQUEST*, int);
void methodDelete(HTTP_REQUEST*, int);
void methodOptions(HTTP_REQUEST*, int);
void methodTrace(HTTP_REQUEST*, int);
void methodConnect(HTTP_REQUEST*, int);
void methodPatch(HTTP_REQUEST*, int);
void methodUnknow(HTTP_REQUEST*, int);

#endif //_METHODS_H_
