#include "include/Packetizer.hpp"

Packetizer::Packetizer(string portName, char control, int baudRate)
{
	this->portName = portName;
	this->control = control;
	this->baudRate = baudRate;
	port = new Serial(portName, baudRate);
	currentState = NO_STATE;
	error = ERR_NO_ERROR;
}

Packetizer::~Packetizer()
{
	delete port;
}

int Packetizer::readPacket(char *buffer)
{
	char byte;
	bool complete = false;

	while (error == ERR_NO_ERROR && !complete)
	{
		switch (currentState)
		{
			case NO_STATE:
				if (port->queryBuffer())
				{
					port->sread(&byte, 1);
					error = ERR_NO_ERROR;
				}
				else
					error = ERR_NOT_ENOUGH_DATA;

				if (byte == control)
				{
					currentState = READ_CTRL;
					error = ERR_NO_ERROR;
				}
				else
					error = ERR_INVALID_CTRL;
				break;

			case READ_CTRL:
				if (port->queryBuffer())
				{
					port->sread(&packetSize, 1);
					error = ERR_NO_ERROR;
					currentState = READ_SIZE;
				}
				else
					error = ERR_NOT_ENOUGH_DATA;
				break;

			case READ_SIZE:
				if (port->queryBuffer() >= packetSize)
				{
					port->sread(buffer, packetSize);
					complete = true;
					error = ERR_NO_ERROR;
				}
				else
					error = ERR_NOT_ENOUGH_DATA;
				break;
		}
	}

	if (complete)
	{
		return packetSize; //always positive or 0
	}
	else
	{
		//Return current error (negative value)
		return error;
	}
}

int Packetizer::writePacket(char *buffer, int bytes)
{
	char *newBuffer = new char[bytes+2];
	newBuffer[0] = control;
	newBuffer[1] = bytes;
	int written = port->swrite(newBuffer, bytes+2);
	delete newBuffer;

	return written;
}

int Packetizer::queryBuffer()
{
	return port->queryBuffer();
}

Packetizer::Packetizer_Error Packetizer::getCurrentError()
{
	return error;
}
