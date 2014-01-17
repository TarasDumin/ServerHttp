#ifndef _REQUEST_H_
#define _REQUEST_H_

#define DEFAULT_TIME_KEAP_ALIVE (10)
#define END_REQUEST             (1)
#define ERROR_RUN               (-1)
#define MAX_LENGTH_METHOD       (16)
#define MAX_LENGTH_VERSION      (11)
#define MAX_LENGTH_STRING       (256)
#define MAX_LENGTH_FIRST_STRING (512)
#define KEY_ISNT_FOUND          (0)
#define SUCCESS                 (1)

typedef enum _YES_NO_QUESTION_
{
    NO_Q,
    YES_Q,
}YES_NO_QUESTION;
typedef struct _REQUEST HTTP_REQUEST;
typedef void (*PARSE_VALUE)(HTTP_REQUEST*, char*, int);
typedef void (*GET_METHOD_HTTP)(HTTP_REQUEST*, char*);
typedef void (*METHOD_HTTP)(HTTP_REQUEST*, int);
typedef enum _HTTP_METHOD_
{
    HTTP_GET,              //Request a representation of the resource.
    HTTP_HEAD,             //Request only the headers for the resource.
    HTTP_POST,             //Process the request body with the resource.
    HTTP_PUT,              //Create or update a new resource with the contents of the request body.
    HTTP_DELETE,           //Remove the specified resource.
    HTTP_OPTIONS,          //Return the HTTP methods the specified resource supports.
    HTTP_TRACE,            //Echo the received request.
    HTTP_CONNECT,          //Convert the connection to a transparent tcp/ip tunnel, usually to allow SSL/TLS through an unencrypted HTTP proxy.
    HTTP_PATCH,            //Apply partial modifications to the resource.
    HTTP_EXTENSION_METHOD  //Anothe value.
}HTTP_METHOD;

typedef enum 
{
	HTTP_1_0,        //HTTP/1.0
	HTTP_1_1,        //HTTP/1.1
	UNDEFINED_HTTP   //Another version
}VERSION;

typedef enum 
{
    /*************************************************Informational****************************************************************/
    HTTP_100_CONTINUE = 100,    	      //The server has received the request headers, and the client should begin to send the request body.
    HTTP_101_SWITCHING,   		      //Protocols The server has received a request to switch protocols and is doing so.
    HTTP_102_PROCESSING,                      // Indicates that the server has received the request, used by WebDAV to avoid timeouts for long-running requests.
    /****************************************************Success*****************************************************************/
    HTTP_200_OK = 200,                        //Standard response for successful HTTP requests.
    HTTP_201_Created,                         //The request was successful, and a new resource has been created.
    HTTP_202_Accepted ,    		      //The request was accepted for processing, but the job hasn't actually been completed.
    HTTP_203_NON_AUTHORITATIVE_INFORMATION,   //The request was successfully processed, but the returned information may be from an untrusted third party.
    HTTP_204_NO_CONTENT,                      //The request was processed and no content is being returned.
    HTTP_206_PARTUAL_CONTENT = 206,           //Only part of the request body is being delivered.
    HTTP_207_MULTY_STATUS,                    //The message body is an XML document and may contain multiple status codes per sub-requests.
    HTTP_208_ALREADY_REPORTED,                //The response has already been enumerated in a previous reply and will not be reported again.
    /*************************************************Redirection*****************************************************************/                         
    HTTP_300_MULTIPLE_CHOICES = 300,          //Indicates that there are multiple locations which the client may follow.
    HTTP_301_MOVED_PERMANENTLY,               //The resource has moved permantly, and all future requests should use the give URL instead.
    HTTP_302_FOUND,                           //The resource has been found (or moved temporarily).
    HTTP_303_SEE_OTHER,                       //The resource has been found, and should be accessed using a GET method. 
    HTTP_304_NOT_MODIFFED,                    //The resource has not been modified since the last time the client has cached it.
    HTTP_305_USE_PROXY,                       //The resource should be accessed through a specified proxy.
    HTTP_307_TEMPORARY_REDIRECT,              //The request should be repeated with the same request method at the given address.
    /************************************************Client Error******************************************************************/
    HTTP_400_BAD_REQUEST = 400,               //The request can not be fulfilled because the request contained bad syntax.
    HTTP_401_UNAUTHORIZED,                    //The client needs to authenticate in order to access this resource.
    HTTP_402_PAYMENT_REQUIRED,                //This code is intended to be used for a micropayment system, but the specifics for this system are unspecified and this code is rarely used.
    HTTP_403_FORBIDDEN,                       //The client is not allowed to access this resource. Generally, the client is authenticated and does not have suficient permission.
    HTTP_404_NOT_FOUND,                       //The resource was not found, though its existence in the future is possible.
    HTTP_405_METHOD_NOT_ALLOWED,              //The method used in the request is not supported by the resource.
    HTTP_406_NOT_ACCEPTABLE,                  //The server can not generate content which is acceptable to the client according to the request's \Accept" header.
    HTTP_407_PROXY_AUTHENTICATION_REQUIRED,   //The client must authenticate with the proxy.
    HTTP_408_REQUEST_TIMEOUT,                 //The client did not complete its request in a reasonable timeframe.
    HTTP_409_CONICT,                          //The request could not be completed due to a conict in state.
    HTTP_410_GONE,                            //The resource is gone, and will always be gone; the client should not request the resource again.
    HTTP_411_LENGTH_REQUIRED,                 //The request is missing its \Content-Length" header, which is required by this resource.
    HTTP_412_PRECONDITION_FAILED,             //The server can not meet preconditions specified in the client request.
    HTTP_413_REQUEST_ENTITY_TOO_LARGE,        //The request body is larger than the server will process.
    HTTP_414_REQUEST_URI_TOO_LONG,            //The request URI is too long for the server to process.
    HTTP_415_UNSUPPORTED_MEDIA_TYPE,          //The server can not process the request body because it is of an unsupported MIME type.
    HTTP_416_REQUESTED_RANGE_NOT_SATISFIABLE, //The client has asked for portion of a file that the server can not supply (ie, a range of bytes outside the size of the requested file).
    HTTP_417_EXPECTATION_FAILED,              //The server can not meet the requirements of the \Expect" header in the request.
    HTTP_418_IM_TEAPOT,                       //Returned by teapots implementing the HyperText Coffee Pot Control Protocol.
    HTTP_420_ENHANCE_YOUR_CALM = 420,         //The client is being rate-limited; a reference to cannabis culture.
    HTTP_422_UNPROCESSABLE_ENTITY = 422,      //The server can not process the request due to semantic errors.
    HTTP_423_LOCKED,                          //The resource is currently locked.
    HTTP_424_FAILED_DEPENDENCY,               //The request failed because of a previously-failed request.
    HTTP_429_TOO_MANY_REQUESTS = 429,         //The client is being rate-limited.
    HTTP_431_REQUEST_HEADER_FIELDS_TOO_LARGE = 431,//Either a single request header is too large, or all the header fields as a group are too large.
    HTTP_444_NO_RESPONSE = 444,               //Used in Nginx logs. Indicates that the server closed the connection without sending any response whatsoever.
    HTTP_449_RETRY_WITH = 449,                //The request should be retried after performing some action.
    HTTP_450_BLOCKED_BY_WINDOWS,              //Windows Parental Controls are turned on and are blocking access to the resource.
    HTTP_451_UNAVAILABLE_FOR_LEGAL_REASONS,   //Intended to be used when a resource is being censored or blocked; a reference to Fahrenheit 451.
    /****************************************************Server Error*****************************************************************/ 
    HTTP_500_INTERNAL_SERVER_ERROR = 500,     //A generic server error message, for when no other more specific message is suitable.
    HTTP_501_NOT_IMPLEMENTED,                 //The server can not process the request method.
    HTTP_502_BAD_GATEWAY,                     //The server is a gateway or proxy, and received a bad response from the upstream server (such as a socket hangup).
    HTTP_503_SERVICE_UNAVAILABLE,             //The resource is temporarily unavailable, usually because it is overloaded or down for maintenence.
    HTTP_504_GATEWAY_TIMEOUT,                 //The server is a gateway or proxy, and the upstream server did not respond in a reasonable timeframe.
    HTTP_505_HTTP_VERSION_NOT_SUPPORTED,      //The server does not support the request's specified HTTP version.
    HTTP_507_INSUFFCIENT_STORAGE = 507,       //The server is out of storage space and can not complete the request.
    HTTP_508_LOOP_DETECTED,                   //The server has detected an infinite loop while processing the request.
    HTTP_509_BANDWIDTH_LIMIT_EXCEEDED         //A convention used to report that bandwidth limits have been exceeded, and not part of any RFC or spec.
}STATUS;

struct _REQUEST
{
    HTTP_METHOD method; 
    VERSION version;
    YES_NO_QUESTION isChunkUsing;
    YES_NO_QUESTION isKeapAliveUsing;
    int timeOfKeapAlive;
    int lenResponse;
    int lenURI;
    int lenBodyResponse;
    
    char *header;
    char *URI;
    char *body;
    STATUS status;

    GET_METHOD_HTTP getMethodHTTP;
    METHOD_HTTP MethodHTTP;
};

int parseParameters(HTTP_REQUEST*, char*, int);
int getValueForKeyString(char*, char*, char*);
int getValueForKeyNumber(char*, char*, int*);
HTTP_REQUEST *createRequestObject(void);
void deleteRequest(HTTP_REQUEST*);

#endif //_REQUEST_H_
