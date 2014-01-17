CC=gcc
CFLAGS=-c -Wall
SOURCES=Server.c workWithSockets.c request.c methods.c debugLogging.c auxiliaryFunction.c accessLoggin.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=invokServ

all: $(SOURCES) $(EXECUTABLE)
    
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@
Server.o: Server.h request.h workWithSockets.h debugLogging.h auxiliaryFunction.h accessLoggin.h
workWithSockets.o: workWithSockets.h debugLogging.h auxiliaryFunction.h accessLoggin.h
debugLogging.o: debugLogging.h 
request.o: request.h methods.h debugLogging.h auxiliaryFunction.h accessLoggin.h
methods.o: methods.h debugLogging.h auxiliaryFunction.h accessLoggin.h
auxiliaryFunction.o: auxiliaryFunction.h accessLoggin.h debugLogging.h workWithSockets.h
accessLoggin.o: accessLoggin.h request.h


.c.o:
	$(CC) $(CFLAGS) $< -o $@
clean:
	rm -rf *.o
