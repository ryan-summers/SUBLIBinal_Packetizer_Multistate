#ifndef PACKETIZER_H
#define PACKETIZER_H

#include "include/Serial.hpp"
#include <string.h>
#include <iostream>



class Packetizer
{
	typedef enum {
		NO_STATE,
		READ_CTRL,
		READ_SIZE
	} State;

public:
	
	typedef enum {
		ERR_NO_ERROR = -1,
		ERR_INVALID_CTRL = -2,
		ERR_NOT_ENOUGH_DATA = -3
	} Packetizer_Error;
	
	Packetizer(string portName = "", char control = '!', int baudRate = 115200);
	~Packetizer();

    int readPacket(char *buffer); //Attempts to read a packet off the serial port
    int writePacket(char *buffer, int bytes); //Writes a packet to the serial port
    int readRaw(char *buffer, int maxBytes); //Attempts to read all data off the serial port
    int writeRaw(char *buffer, int bytes); //Writes all specified bytes to the serial port

    bool open();
    bool close();
	
	int queryBuffer(); //Queries number of bytes available on the serial port
	Packetizer_Error getCurrentError();



private:
	int baudRate;
	string portName;
	char control;
	Serial *port;
	State currentState;
	char packetSize;
	Packetizer_Error error;
};

#endif
