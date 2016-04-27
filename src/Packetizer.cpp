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

bool Packetizer::open()
{
    return port->sopen();
}

bool Packetizer::close()
{
    return port->sclose();
}
static int errCnt = 0;
int Packetizer::readPacket(char *buffer)
{
	char byte;
	bool complete = false;
    error = ERR_NO_ERROR;

	while (error == ERR_NO_ERROR && !complete)
	{
		switch (currentState)
		{
			case NO_STATE:
				if (port->queryBuffer())
				{
                    port->sread(&byte, 1);

                    if (byte == control)
                    {
                        currentState = READ_CTRL;
                        error = ERR_NO_ERROR;
                    }
                    else
                    {
                        error = ERR_INVALID_CTRL;
                    }
				}
				else
					error = ERR_NOT_ENOUGH_DATA;

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
                    int cnt = port->sread(buffer, packetSize);
                    if (cnt == packetSize)
                    {
                        complete = true;
                        error = ERR_NO_ERROR;
                    }
                    else
                        error = ERR_NOT_ENOUGH_DATA;
				}
				else
					error = ERR_NOT_ENOUGH_DATA;
				break;
		}
	}

	if (complete)
    {
        currentState = NO_STATE;
		return packetSize; //always positive or 0
	}
	else
	{
        if (error == ERR_INVALID_CTRL)
            errCnt++;
		//Return current error (negative value)
		return error;
        std::cout << errCnt << std::endl;
	}
}

int Packetizer::readRaw(char *buffer, int maxBytes)
{
    int bytesAvailable = port->queryBuffer();

    if (bytesAvailable < maxBytes)
        return port->sread(buffer, bytesAvailable);
    else
        return port->sread(buffer, maxBytes);
}

int Packetizer::writePacket(char *buffer, int bytes)
{
	char *newBuffer = new char[bytes+2];
	newBuffer[0] = control;
	newBuffer[1] = bytes;
    memcpy(&(newBuffer[2]), buffer, bytes);
	int written = port->swrite(newBuffer, bytes+2);
	delete newBuffer;

	return written;
}

int Packetizer::writeRaw(char *buffer, int bytes)
{
    return port->swrite(buffer, bytes);
}

int Packetizer::queryBuffer()
{
	return port->queryBuffer();
}

Packetizer::Packetizer_Error Packetizer::getCurrentError()
{
	return error;
}
