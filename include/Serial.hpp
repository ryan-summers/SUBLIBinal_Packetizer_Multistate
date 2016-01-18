#ifdef __linux

typedef int HANDLE; //HANDLE is used within windows. For linux, just typedef it as an int

#include <termios.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#define INVALID_HANDLE_VALUE 0

#elif _WIN32

#include <windows.h> //Include the main windows header
#include <WinBase.h>

#endif

#include <string>
#include <stdio.h>
	using namespace std;

class Serial 
{
	public:
		Serial(string portName = "", int baudRate = 0);

		~Serial(); //Default destructor for the serial port

		bool sopen(); //Open the serial port
		bool sclose(); //Close the serial port
		int sread(char *buffer, int bytes); //Read data from the port
		int swrite(char *buffer, int bytes); //Write data to the port
		int queryBuffer(); //Query how many bytes are available on the port

	protected:
		bool configure();

	private:
		HANDLE portHandle; //The handle to the serial port
		string portName;
		int baudRate;		
};
