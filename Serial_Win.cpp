
/*
/
/
/-----------------------------------THIS MODULE IS OUT-OF-DATE------------------------------------
/
/
/ MODULE COMMUNICATION CLASS
/
/ This class communicate with a serial port and reads all the data received through TX.
/ it communicates with external hardware modules to read its' data, it is currently used
/ to read GPS NMEA setences. This class only used in windows operating system by using
/ Win32 API
/
/ NOTES :
/ 1) Variables begin with underscor _ are private members of the class
/ 2) To use this class you need to call functions in the following order:
/   A. serial_init()
/	B. flush_file_buffer();
/	C. set_COM_timeouts();
/	D. DCB_init();
/	D.2 (Optional) event_driven_read();
/   E. read_line();
/   F. serial_close();
*/

#if defined (_WIN32)

#include "Serial_Handle.h"

/*
/ DESCRIPTION;
/ Contructor for the class, it receives COM number from the user
/
/ RETURN:
/ None
*/

Serial_Handle::Serial_Handle() {

}

/*
/ DESCRIPTION;
/ Creates a handle for the communication serial port
/
/ RETURN:
/ If successful, it returns 1, if not, returns -1
*/

INT_16 Serial_Handle::serial_init(string COM_num, int _baud_rate) {

	this->_baud_rate = _baud_rate;
	this->_COM_num = COM_num.c_str();

	_com_handle = CreateFileA(_COM_num,    // serial com name
		GENERIC_READ | GENERIC_WRITE, // Acess reading and writing
		0,                            // sharing mode : none
		NULL,                         // Security : none
		OPEN_EXISTING,				  // creatin disposition
		0,							  // non overlapped attribute
		NULL);						  // templete file

	if (_com_handle == INVALID_HANDLE_VALUE) {
		print_error(_COM_num);
		CloseHandle(_com_handle);
		return -1;
	}
	
	return 1;
}

/*
/ DESCRIPTION;
/ Flushes the file buffer to remove any remaining data
/
/ RETURN:
/ If successful, it returns 1, if not, returns -1
*/

INT_16 Serial_Handle::flush_file_buffer() {
	_success = FlushFileBuffers(_com_handle);
	if (!_success)
	{
		print_error("Failed to flush serial port");
		CloseHandle(_com_handle);
		return -1;
	}
	
	return 1;
}

/*
/ DESCRIPTION;
/ Sets the time-out to wait for bytes read till it exits
/
/ RETURN:
/ If successful, it returns 1, if not, returns -1
*/

INT_16 Serial_Handle::set_COM_timeouts() {
	_timeouts = { 0 };

	_timeouts.ReadIntervalTimeout = 50;       // In milli-seconds
	_timeouts.ReadTotalTimeoutConstant = 50; 
	_timeouts.ReadTotalTimeoutMultiplier = 10; 

	_success = SetCommTimeouts(_com_handle, &_timeouts);
	if (!_success)
	{
		print_error("Failed to set serial timeouts");
		CloseHandle(_com_handle);
		return -1;
	}
	
	return 1;
}

/*
/ DESCRIPTION;
/ Initializes the device control block to configure baud rate
/ and other paramaters
/
/ RETURN:
/ If successful, it returns 1, if not, returns -1
*/

INT_16 Serial_Handle::DCB_init() {

	// Initializing DCB structure
	_dcb_params = { 0 };
	_dcb_params.DCBlength = sizeof(_dcb_params);

	_success = GetCommState(_com_handle, &_dcb_params);
	if (!_success)
	{
		print_error("Failed to get serial settings");
		CloseHandle(_com_handle);
		return -1;
	}

	// TODO: Allow other Baud rate from the programmer
	switch (_baud_rate) {
		case 110:
			_dcb_params.BaudRate = CBR_110;
			break;
		case 300:
			_dcb_params.BaudRate = CBR_300;
			break;
		case 600:
			_dcb_params.BaudRate = CBR_600;
			break;
		case 1200:
			_dcb_params.BaudRate = CBR_1200;
			break;
		case 2400:
			_dcb_params.BaudRate = CBR_2400;
			break;
		case 4800:
			_dcb_params.BaudRate = CBR_4800;
			break;
		case 9600:
			_dcb_params.BaudRate = CBR_9600;
			break;
		case 14400:
			_dcb_params.BaudRate = CBR_14400;
			break;
		case 19200:
			_dcb_params.BaudRate = CBR_19200;
			break;
		case 38400:
			_dcb_params.BaudRate = CBR_38400;
			break;
		case 57600:
			_dcb_params.BaudRate = CBR_57600;
			break;
		case 115200:
			_dcb_params.BaudRate = CBR_115200;
			break;
	}

	_dcb_params.ByteSize = 8;           // ByteSize = 8
	_dcb_params.StopBits = ONESTOPBIT;  // StopBits = 1
	_dcb_params.Parity = NOPARITY;      // Parity = None

	_success = SetCommState(_com_handle, &_dcb_params);
	if (!_success)
	{
		print_error("Failed to set serial settings");
		CloseHandle(_com_handle);
		return -1;
	}
	
	return 1;
}

/*
/ DESCRIPTION;
/ Use this function if you want event driven instead of polling
/
/ RETURN:
/ If successful, it returns 1, if not, returns -1
*/

INT_16 Serial_Handle::event_driven_read() {
	_success = SetCommMask(_com_handle, EV_RXCHAR);

	if (!_success)
	{
		print_error("Failed to set event driven");
		CloseHandle(_com_handle);
		return -1;
	}
	

	DWORD dwEventMask;
	_success = WaitCommEvent(_com_handle, &dwEventMask, NULL);

	if (!_success)
	{
		print_error("Failed to wait for event");
		CloseHandle(_com_handle);
		return -1;
	}
	
	return 1;
}

/*
/ DESCRIPTION;
/ Read every line that begins with '$' and ends with '\n'
/
/ RETURN:
/ If successful, it returns 1 and the NMEA sentence, if not, returns -1
*/

// ----------------[DEPRECATED]------------------ //

string Serial_Handle::read_line()
{
	DWORD received;
	string SerialBuffer;
	char buffer;
	bool linebegin = false;

	do {
		BOOL success = ReadFile(_com_handle,    //Handle of the Serial port
			&buffer,							//character to hold the read byte
			sizeof(buffer),						//Size of character
			&received,							//Number of bytes read
			NULL);
		if (!success)
		{
			print_error("Failed to read from com port");
			return "-1";
		}

		if (buffer != '$' && !linebegin) {
			continue;
		}
		else {
			// TODO: Enters here everytime, needs to fix
			linebegin = true;
		}

		SerialBuffer.append(1, buffer);
	} while (buffer != '\n');

	return SerialBuffer;
}

/*
/ DESCRIPTION;
/ Read 5 lines at a time that begins with '$' and ends with '\n'
/
/ RETURN:
/ If successful, it returns 1 and the 5 NMEA sentences, if not, returns -1
*/

string Serial_Handle::read_line_wc()
{
	DWORD received;
	string SerialBuffer;
	char buffer;
	bool linebegin = false;
	int counter = 0;

	do {
		BOOL success = ReadFile(_com_handle,    //Handle of the Serial port
			&buffer,							//character to hold the read byte
			sizeof(buffer),						//Size of character
			&received,							//Number of bytes read
			NULL);
		if (!success)
		{
			print_error("Failed to read from com port");
			return "-1";
		}

		if (buffer != '$' && !linebegin) {
			continue;
		}
		else {
			// TODO: Enters here everytime, needs to fix
			linebegin = true;
		}

		// increase counter
		if (buffer == '\n')
			counter++;

		SerialBuffer.append(1, buffer);
	} while (counter < 5);

	return SerialBuffer;
}

/*
/ DESCRIPTION;
/ Closes the serial port HANDLE to clear the memory
/
/ RETURN:
/ If successful, it returns 1, if not, returns -1
*/

INT_16 Serial_Handle::serial_close() {
	_success = CloseHandle(_com_handle);         //Closing the Serial Port
	if (!_success)
	{
		print_error("Failed to close handle");
		CloseHandle(_com_handle);
		return -1;
	}
	return 1;
}

/*
/ DESCRIPTION;
/ Prints error massages in case any function returns -1
/
/ RETURN:
/ None
*/

void Serial_Handle::print_error(const char* user_msg)
{
	DWORD dw = GetLastError();
	char buffer[256];

	DWORD size = FormatMessageA(
		FORMAT_MESSAGE_FROM_SYSTEM |
		FORMAT_MESSAGE_MAX_WIDTH_MASK,					// Specifiy source and maximum width
		NULL,
		dw,
		MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_US),	// Language ID
		buffer,										    // Buffer to contain the message
		sizeof(buffer),									// Size of buffer
		NULL);

	if (size == 0) { buffer[0] = 0; }
	fprintf(stderr, "%s: %s\n", user_msg, buffer);
}

/*
/ DESCRIPTION;
/ Destructor for the class
/
/ RETURN:
/ None
*/
Serial_Handle::~Serial_Handle() {}

#endif