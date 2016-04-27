#ifndef SERIAL_H
#define SERIAL_H

#ifdef __linux

typedef int HANDLE; //HANDLE is used within windows. For linux, just typedef it as an int

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/tty.h>
#define INVALID_HANDLE_VALUE 0

#elif _WIN32

#include <windows.h> //Include the main windows header
#include <WinBase.h>

#define B115200 115200
#endif

#include <string.h>
#include <string>
#include <stdio.h>

using namespace std;

class Serial 
{
	public:
        Serial(string portName = "", int baudRate = B115200);

		~Serial(); //Default destructor for the serial port

		bool sopen(); //Open the serial port
		bool sclose(); //Close the serial port
		int sread(char *buffer, int bytes); //Read data from the port
		int swrite(char *buffer, int bytes); //Write data to the port
        int sflush(); //flush the buffer
		int queryBuffer(); //Query how many bytes are available on the port
        int setRTS(int state); //Set the RTS to the specified state

	protected:
		bool configure();

	private:
		HANDLE portHandle; //The handle to the serial port
		string portName;
		int baudRate;		
};

#endif
