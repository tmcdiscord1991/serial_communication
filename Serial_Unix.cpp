
/*
/ MODULE COMMUNICATION CLASS
/
/ This class communicate with a serial port and reads all the data received through TX.
/ it communicates with external hardware modules to read its' data, it is currently used
/ to read GPS NMEA setences. This class only used in unix operating system
/
/
/ NOTES :
/ 1) Variables begin with underscor _ are private members of the class
/ 2) To use this class you need to call functions in the following order:
/   A. serial_init();
/   B. set_COM_timeouts();
/	C. serial_param_init();
/	D. config_cons();
/   E. read_line();
/   F. serial_close();
*/


#if defined (__unix__)
#include "Serial_Unix.h"
//#include <stdio.h>

/*
/ DESCRIPTION;
/ Contructor for the class, it receives COM number from the user
/
/ RETURN:
/ None
*/

Serial_Unix::Serial_Unix() {

	// SETTING DEFAULT CONFIGURATIN VALUES
	// -----------------------------------
	this->config_unix.baud_rate = 9600;
	this->config_unix.bits_per_byte = 8;
	this->config_unix.parity_bit = config_unix.NONE_BIT;
	this->config_unix.stop_field = config_unix.ONE;
	this->config_unix.flow_control = config_unix.NONE;
	this->config_unix.read_mode = config_unix.NON_CANONICAL;
	this->config_unix.is_init = true;


}

/*
/ DESCRIPTION;
/ Creates a handle for the communication serial port
/
/ RETURN:
/ If successful, it returns 1, if not, returns -1
*/

int8_t Serial_Unix::serial_init(string COM_num, int _baud_rate) {

	// Set the class instances passed through init()
	this->_baud_rate = _baud_rate;
	this->_COM_num = COM_num.c_str();


	// open read and write serial port
	this->_serial_com = open(_COM_num, O_RDWR);
	if (_serial_com < 0) {
		print_error("Error while opening device...");
		exit(1);
	}

	return 1;
}

int8_t Serial_Unix::get_tc_attributes() {

	// Save current settings, and handle errors
	/*
	/ tc : terminal control/communication
	/ cflag : control flags
	*/

	if (tcgetattr(_serial_com, &serial_config_old) < 0) {
		print_error("Error in getting terminal attributes");
		serial_close();
		exit(1);
	}

	/*
	/ Clear termios buffer for new settings
	/ --------------------------------------
	/ Note: if you clear buffer, no need to use bitwise operators
	/ to keep old attributes
	*/
	//bzero(&serial_config, sizeof(serial_config));

	return 1;
}

/*
/ DESCRIPTION;
/ it containts the handle of the serial_port
/ if the developer wants to add more configurations
/
/ RETURN:
/ A pointer to the serial_port handle
*/

int Serial_Unix::serial_handle() {

	return _serial_com;
}

/*
/ DESCRIPTION;
/ Restoring old termios attributes
/
/ RETURN:
/ If successful, it returns 1, if not, prints error
*/

int8_t Serial_Unix::reset_term_set() {

	_success = tcsetattr(_serial_com, TCSANOW, &serial_config_old);
	if (_success < 0) {
		print_error("Error in setting termios attributes");
		serial_close();
		exit(1);
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

int8_t Serial_Unix::set_COM_timeouts() {

	/* c_cc : control characters.
	/ -------------------------------------------------
	/ Defines the control characters for end of file,
	/ stop, etc...
	/ -------------------------------------------------
	/ VTIME and VMIN are used for Non-Canonical mode :
	/ read() will return number of bytes requested
	/ for returning only when there's new-line, eof or
	/ eol, use Canonical Mode
	/ -------------------------------------------------
	/ VMIN : minmum number of bytes must be available for
	/ in queue for read to return
	/ VTIME : timeouts before returning
	*/
	serial_config.c_cc[VTIME] = 1;
	serial_config.c_cc[VMIN] = 0;

	return 1;
}

/*
/ DESCRIPTION;
/ Initializes the serial configuration to set baud rate
/ and other paramaters
/
/ RETURN:
/ If successful, it returns 1, if not, returns -1
*/

int8_t Serial_Unix::serial_param_init() {

	/* c_cflag : control mode bit-flag.
	/ -------------------------------------------------
	/ Contains settings for the port, as the baudrate,
	/ bits per character, stop bits..etc.
	/ -------------------------------------------------
	*/

	if (config_unix.parity_bit == config_unix.NONE_BIT)
		serial_config.c_cflag &= ~PARENB;				// Clear parity bit, parity = none

	if (config_unix.stop_field == config_unix.ONE)
		serial_config.c_cflag &= ~CSTOPB;				// Clear stop field, 1 stop bit

	serial_config.c_cflag &= ~CSIZE;					// Clear all bits that set the data size 

	if (config_unix.bits_per_byte == 8)
		serial_config.c_cflag |= CS8;					// 8 bits per byte 

	// Use RTS and CTS pin here
	serial_config.c_cflag &= ~CRTSCTS; // disable RTS/CTS

	serial_config.c_cflag |= CREAD // read receiving characters
		| CLOCAL; // CLOCAL : local connection, no modem

// Set the baud rate choosen by user
	switch (config_unix.baud_rate) {
	case 0:
		cfsetispeed(&serial_config, B0);
		cfsetospeed(&serial_config, B0);
		break;
	case 50:
		cfsetispeed(&serial_config, B50);
		cfsetospeed(&serial_config, B50);
		break;
	case 75:
		cfsetispeed(&serial_config, B75);
		cfsetospeed(&serial_config, B75);
		break;
	case 110:
		cfsetispeed(&serial_config, B110);
		cfsetospeed(&serial_config, B110);
		break;
	case 134:
		cfsetispeed(&serial_config, B134);
		cfsetospeed(&serial_config, B134);
		break;
	case 150:
		cfsetispeed(&serial_config, B150);
		cfsetospeed(&serial_config, B150);
		break;
	case 200:
		cfsetispeed(&serial_config, B200);
		cfsetospeed(&serial_config, B200);
		break;
	case 300:
		cfsetispeed(&serial_config, B300);
		cfsetospeed(&serial_config, B300);
		break;
	case 600:
		cfsetispeed(&serial_config, B600);
		cfsetospeed(&serial_config, B600);
		break;
	case 1200:
		cfsetispeed(&serial_config, B1200);
		cfsetospeed(&serial_config, B1200);
		break;
	case 1800:
		cfsetispeed(&serial_config, B1800);
		cfsetospeed(&serial_config, B1800);
		break;
	case 2400:
		cfsetispeed(&serial_config, B2400);
		cfsetospeed(&serial_config, B2400);
		break;
	case 4800:
		cfsetispeed(&serial_config, B4800);
		cfsetospeed(&serial_config, B4800);
		break;
	case 9600:
		// Set input and output baud rate to be 9600
		cfsetispeed(&serial_config, B9600);
		cfsetospeed(&serial_config, B9600);
		break;
	case 19200:
		cfsetispeed(&serial_config, B19200);
		cfsetospeed(&serial_config, B19200);
		break;
	case 38400:
		cfsetispeed(&serial_config, B38400);
		cfsetospeed(&serial_config, B38400);
		break;
	default:
		cfsetispeed(&serial_config, B9600);
		cfsetospeed(&serial_config, B9600);
		config_unix.baud_rate = 9600;
		break;
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

void Serial_Unix::config_canon() {

	/* c_lflag : local mode bit-flags
	/ -------------------------------------------------
	/ determine if characters are echoed, signals are
	/ sent to your program, etc...
	/ -------------------------------------------------
	*/

	if (config_unix.read_mode == config_unix.NON_CANONICAL) {
		//------ FOR NON-CANONICAL MODE --------
		serial_config.c_lflag &= ~ICANON; // disable canonical mode 
		serial_config.c_lflag &= ~ECHO; // disable echo
		serial_config.c_lflag &= ~ECHOE; // disable echo erasure
		serial_config.c_lflag &= ~ECHONL; // disable echo new line
		// ---------------------------------
	}

	serial_config.c_lflag &= ~ISIG;
	serial_config.c_iflag &= ~IGNPAR; // ignora bytes that has parity errors

	if (config_unix.flow_control == config_unix.NONE)
		serial_config.c_iflag &= ~(IXON | IXOFF | IXANY); // Turn off flow controls (sotware handshaking)

	serial_config.c_iflag &= ~(IGNBRK
		| BRKINT
		| PARMRK
		| ISTRIP
		| INLCR // map nl to cr
		| IGNCR
		| ICRNL);


	/* c_oflag : output mode bit-flag
	/ -------------------------------------------------
	/ handles output processing before being sent
	/ -------------------------------------------------
	*/
	serial_config.c_oflag &= ~OPOST;
	serial_config.c_oflag &= ~ONLCR;

	/* c_iflag : input mode bit-flag
	/ -------------------------------------------------
	/ handles input processing before being read with read()
	/ -------------------------------------------------
	*/

	return;
}

int8_t Serial_Unix::set_tc_attributes() {

	// Save serial settings and check for errors
	flush_buffer();
	if (tcsetattr(_serial_com, TCSANOW, &serial_config) < 0) {
		print_error("Error in setting termios attributes");
		serial_close();
		exit(1);
	}

	return 1;
}

/*
/ DESCRIPTION;
/ Flushes the read and write buffer
/
/ RETURN:
/ If successful, it returns 1, if not, prints error and exit
*/

int8_t Serial_Unix::flush_buffer() {
	_success = tcflush(_serial_com, TCIOFLUSH);
	if (_success < 0) {
		print_error("Failed to flush R/W buffer");
		serial_close();
		exit(1);
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

// ------------------- FOR DEBUGGING GPS ----------------------//

string Serial_Unix::read_line_dbg() {
	// allocate memeory for read buffer
	char buffer[1];

	//Use this for debugging to set all string to \0
	memset(&buffer, '\0', sizeof(buffer));


	// allocating variables for processing the buffer
	int received;
	string SerialBuffer;
	bool linebegin = false;
	int counter = 0;

	do {

		// read one character at a time with read function
		received = read(_serial_com,
			&buffer,
			sizeof(buffer));

		// returns -1 if there's an error
		// TODO : figure out why when first starts it give an error
		// needs adminstrator right
		if (received < 0) {
			print_error("Error reading");
			serial_close();
			exit(1);
		}

		// check for gps headers strting with $ character
		if (buffer[0] != '$' && !linebegin) {
			continue;
		}
		else {
			linebegin = true;
		}

		// store the character in a string 
		SerialBuffer.append(1, buffer[0]);
	} while (buffer[0] != '\n');

	return SerialBuffer;
}


/*
/ DESCRIPTION;
/ Read 5 lines at a time that begins with '$' and ends with '\n'
/
/ RETURN:
/ If successful, it returns 1 and the 5 NMEA sentences, if not, returns -1
*/

// ------------------- FOR DEBUGGING GPS ----------------------//

string Serial_Unix::read_line_gps() {
	// allocate memeory for read buffer
	char buffer;

	//Use this for debugging to set all string to \0
	//memset(&buffer, '\0', sizeof(buffer));


	// allocating variables for processing the buffer
	int received;
	string SerialBuffer;
	bool linebegin = false;
	int counter = 0;

	do {

		// read one character at a time with read function
		received = read(_serial_com,
			&buffer,
			sizeof(buffer));

		// returns -1 if there's an error
		// TODO : figure out why when first starts it give an error
		// needs adminstrator right
		if (received < 0) {
			print_error("Error reading");
			serial_close();
			exit(1);
		}

		// check for gps headers strting with $ character
		if (buffer != '$' && !linebegin) {
			continue;
		}
		else {
			linebegin = true;
		}

		// increase counter for every new line feed
		if (buffer == '\n')
			counter++;

		// store the character in a string 
		SerialBuffer.append(1, buffer);
	} while (counter < 4);

	return SerialBuffer;
}

bool Serial_Unix::set_config_unix(Serial_Config const& config) {

	this->config_unix = config;

	this->get_tc_attributes(); // get attributes first

	this->set_COM_timeouts();
	this->serial_param_init();
	this->config_canon();

	this->set_tc_attributes(); // set attributes last

	return 1;
}


Serial_Config& Serial_Unix::get_config_unix() {
	return this->config_unix;
}


/*
/ DESCRIPTION;
/ Closes the serial port to clear the memory
/
/ RETURN:
/ If successful, it returns 1, if not, returns -1
*/

int8_t Serial_Unix::serial_close() {
	_success = close(_serial_com);

	if (_success < 0) {
		print_error("Failed to close serial port");
		exit(1);
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

void Serial_Unix::print_error(string user_msg)
{
	std::cerr << user_msg << std::endl;
	std::cerr << "Error No." << errno << std::endl;
	std::perror("Error is");
}

/*
/ DESCRIPTION;
/ Destructor for the class
/
/ RETURN:
/ None
*/
Serial_Unix::~Serial_Unix() {}
#endif