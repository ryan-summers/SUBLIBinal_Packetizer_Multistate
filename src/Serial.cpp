#include "include/Serial.hpp"

Serial::Serial(string portName, int baudRate)
{
	portHandle = NULL;
	this->portName = portName;
	this->baudRate = baudRate;
}

Serial::~Serial()
{
	this->sclose();
}


int Serial::sread(char *buffer, int bytes)
{
#ifdef __linux
	return read(portHandle, buffer, bytes);
#elif _WIN32
    int bytesRead;
    int result = ReadFile(portHandle, buffer, bytes, (LPDWORD)&bytesRead, 0);
    return bytesRead;
#endif
}

int Serial::swrite(char *buffer, int bytes)
{
#ifdef __linux
	return write(portHandle, buffer, bytes);
#elif _WIN32
    int bytesWritten;
    int result = WriteFile(portHandle, buffer, bytes, (LPDWORD)&bytesWritten, 0);
    return bytesWritten;
#endif
}

bool Serial::sclose()
{
	if (portHandle)
    {
#ifdef __linux
		close(portHandle);
#elif _WIN32
        CloseHandle(portHandle);
#endif
		return true;
	}
	else
	{
		return false;
	}
}

#ifdef __linux
bool Serial::sopen()
{
	portHandle = open(
			portName.c_str(),
			O_RDWR | O_NDELAY | O_NOCTTY | O_ASYNC
			);	

	if (portHandle == INVALID_HANDLE_VALUE)
		return false;
	else
		return (this->configure());
}

bool Serial::configure()
{
	struct termios portConfig = {0}; //initialize our termios structure

	if (tcgetattr(portHandle, &portConfig) != 0) {
		printf("Get Attributes failed.\n Error: %d", errno);
		return false;
	}

	//manually configure the port flags
	portConfig.c_oflag = 0; //do not filter the output of the serial port
	portConfig.c_lflag = 0; //Do not use signalling characters

	portConfig.c_cflag |= (portConfig.c_cflag & ~CSIZE) | CS8; //Set 8 bit
	portConfig.c_cflag |= CREAD | CLOCAL; //8 bit, enable receiver, ignore modem controls	
	portConfig.c_cflag &= ~(PARENB | PARODD); //Disable parity
	portConfig.c_cflag &= ~(CSTOPB); //Set 1 stop bit
	portConfig.c_cflag &= ~(CRTSCTS); //Disable RTS CTS flow control

    //portConfig.c_iflag |= IGNBRK | IGNPAR | IGNCR; //ignore break, parity errors, and carriage returns
	portConfig.c_iflag &= ~(IXON | IXOFF | IXANY);

	cfsetospeed(&portConfig, baudRate);
	cfsetispeed(&portConfig, baudRate);

	portConfig.c_cc[VMIN] = 255; //the port will not return until timeout occurs or bytes requested are red OR 255 (whichever is less)
	portConfig.c_cc[VTIME] = 5; //the port has .5 second timeout after first byte

	if (tcsetattr(portHandle, TCSANOW,  &portConfig) != 0)
	{	
		printf("Failed to set port attributes. Error: %d\n", errno);
		return false;		
	}

	return true; //successful completion
}

int Serial::queryBuffer()
{
	int bytesAvailable;
	ioctl(portHandle, FIONREAD, &bytesAvailable);
	return bytesAvailable;
}

#elif _WIN32
bool Serial::sopen()
{
    portHandle = CreateFileA(
			portName.c_str(),
			GENERIC_READ | GENERIC_WRITE,
			0,
			0,
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0
			);

	if (portHandle == INVALID_HANDLE_VALUE)
		return false;
	else
		return this->configure();
}

bool Serial::configure()
{
	DCB config = {0};
	config.DCBlength = sizeof(config);

    if (GetCommState(portHandle, &config) == 0) {
		printf("Unable to get configuration of port. Error: %d", GetLastError());
		return false;
	}

    config.BaudRate = baudRate;
	config.StopBits = ONESTOPBIT;
	config.Parity = NOPARITY;
	config.ByteSize = 8;
	config.fDtrControl = 0;
	config.fRtsControl = 0;

    if (SetCommState(portHandle, &config) == 0) {
		printf("failed to set port configuration. Error: %d\n", GetLastError());
		return false;
	}

	return true;
}

int Serial::queryBuffer()
{
	COMSTAT stat;
	ClearCommError(portHandle, NULL, &stat);
	return stat.cbInQue;
}

#endif

#ifdef __linux
int Serial::sflush()
{
    tcflush(this->portHandle,TCIOFLUSH);
}
#elif _WIN32

#endif
