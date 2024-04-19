#include "Serial_Module.h"


Serial_Module::Serial_Module() {}


// TODO: add default configurations in constructor
// TOCHECK: Does constructor run first before _serial_unix instance?
Serial_Module::Serial_Module(string const& _com_num) {

#if defined (_WIN32)

	// win code here
	_serial_win.serial_init(_com_num, 9600);
	_serial_win.flush_file_buffer();
	_serial_win.set_COM_timeouts();
	_serial_win.DCB_init();
	_serial_win.event_driven_read();

#elif defined (__unix__) 

	// unix here
	_serial_unix.serial_init(_com_num, 9600);

	_serial_unix.get_tc_attributes(); // get attributes first

	_serial_unix.set_COM_timeouts();
	_serial_unix.serial_param_init();
	_serial_unix.config_canon();

	_serial_unix.set_tc_attributes(); // set attributes last

	poll_serial = (pollfd*)malloc(sizeof(pollfd));

	// initialize poll_serial for event-driven here
	/*Error : error polling when used here*/

	/*this->serial_handle = _serial_unix.serial_handle();
	this->poll_serial.fd = serial_handle;
	this->poll_serial.events |= POLLIN;
	this->num_fds = 1;*/



#endif

}

int8_t Serial_Module::init(string const& _com_num) {

#if defined (_WIN32)

	// win code here
	_serial_win.serial_init(_com_num, 9600);
	_serial_win.flush_file_buffer();
	_serial_win.set_COM_timeouts();
	_serial_win.DCB_init();
	_serial_win.event_driven_read();

#elif defined (__unix__) 

	// unix here
	_serial_unix.serial_init(_com_num, 9600);

	_serial_unix.get_tc_attributes(); // get attributes first

	_serial_unix.set_COM_timeouts();
	_serial_unix.serial_param_init();
	_serial_unix.config_canon();

	_serial_unix.set_tc_attributes(); // set attributes last

	poll_serial = (pollfd*)malloc(sizeof(pollfd));



#endif

	return 1;
}

// --------------- READING METHODS ------------- //


unsigned int Serial_Module::bytes_available() {

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	unsigned int bytes_available;

	//TODO: Fix the FIONREAD not found to be able to manage buffers
	::ioctl(_serial_com, FIONREAD, &bytes_available);

	return bytes_available;
}


/*
/ DESCRIPTION;
/ Reads one byte/char from the serial port
/
/ RETURN:
/  char read if successful, otherwise, prints an error
*/

char Serial_Module::read() {
#if defined (__unix__)

	serial_handle = _serial_unix.serial_handle();
	poll_serial->fd = serial_handle;
	poll_serial->events |= POLLIN;
	num_fds = 1;


	// allocate memeory for read buffer
	//TODO: make programmer pass *buffer instead if necessary
	char char_buffer;

	// allocating variables for processing the buffer
	int received;

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	// poll event waiting for input
	poll_success = poll(poll_serial, num_fds, -1);  //-1 return immediately, no timeouts

	if (poll_success < 0) {
		_serial_unix.print_error("Error polling input buffer");
	}

	else if (poll_success > 0) {

		if (poll_serial->revents != 0) {

			if (poll_serial->revents & POLLIN) {

				// read one character at a time with read function
				received = ::read(serial_handle,
					&char_buffer,				// Reference to char_buffer
					sizeof(char_buffer));		// size of char_buffer

				// returns -1 if there's an error
				if (received < 0) {
					_serial_unix.print_error("Error reading character");
				}
			}
			else {
				std::clog << "poll error revents" << std::endl;
				exit(1);
			}

		}
		else {
			std::clog << "poll error" << std::endl;
			exit(1);
		}
	}

	return char_buffer;

#elif defined (_WIN32)
	// win32 code here
	return '';
#endif
}


/*
/ DESCRIPTION;
/ overloaded method, Reads many characters from the serial ports
/
/ RETURN:
/  string of characters read if successful, otherwise, prints an error
*/

string Serial_Module::read(size_t num_chars) {
#if defined (__unix__)

	serial_handle = _serial_unix.serial_handle();
	poll_serial->fd = serial_handle;
	poll_serial->events |= POLLIN;
	num_fds = 1;



	//temp char memory
	char char_buffer = 0;

	// allocate memeory for read buffer
	string string_buffer = "";

	// allocating variables for processing the buffer
	int received = 0;

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	do {
		// poll event waiting for input
		poll_success = poll(poll_serial, num_fds, -1);  //-1 return immediately, no timeouts

		if (poll_success < 0) {
			_serial_unix.print_error("Error polling input buffer");
		}

		else if (poll_success > 0) {

			if (poll_serial->revents != 0) {

				if (poll_serial->revents & POLLIN) {

					// read one character at a time with read function
					received = ::read(_serial_com,
						&char_buffer,				// Reference to char_buffer
						sizeof(char_buffer));		// size of char_buffer

					// returns -1 if there's an error
					if (received < 0) {
						_serial_unix.print_error("Error reading string");
					}

					// append to the string buffer
					string_buffer.append(1, char_buffer);

					//decrease num_chars
					--num_chars;
				}
				else {
					std::clog << "poll error revents" << std::endl;
					exit(1);
				}

			}
			else {
				std::clog << "poll error" << std::endl;
				exit(1);
			}
		}

	} while (num_chars > 0);

	return string_buffer;

#elif defined (_WIN32)
	//win32 code here
	return "";
#endif
}

/*
/ DESCRIPTION;
/ Reads a string starts with a specifc character and
/ ends with a specific character, with number of instances
/ requried
/
/ PARAMETERS:
/ starts_with : the first character for string starts with
/ ends_with : last character it ends with
/ num_instances : the number of times to store the pattern
/
/ RETURN:
/  characters read if successful, otherwise, prints an error
*/

string Serial_Module::read_pattern(char starts_with, char ends_with, unsigned int num_instances) {
#if defined (__unix__)

	/* TODO: Optimize this method to store all characters pulled
	* instead of taking one character at a time.
	*
	*/

	if (num_instances < 1) {
		cerr << "Please enter a number of instances larger than 0";
		return "-1";
	}


	// allocate memeory for read buffer
	char buffer;

	// allocating variables for processing the buffer
	int received;
	string SerialBuffer;
	bool pat_begin = false;
	int counter = 0;

	// handle to serial_unix
	// already assigned in the init method
	// handle to serial_unix
	// int _serial_com = _serial_unix.serial_handle();

	// initialize poll_serial for event-driven here
	/*TODO: after resetting configuratin, this handle will change value*/
	//struct pollfd* poll_serial_r;

	/*
	* TODO : Apply poll method to the rest of the reading methods to avoid input/output errors
	*
	*/

	serial_handle = _serial_unix.serial_handle();  /* SERIOUS BUG: Discover why when you remove those statements, the program exit immediately*/
	poll_serial->fd = serial_handle;
	poll_serial->events |= POLLIN;
	num_fds = 1;

	do {

		// Poll event waiting for input
		/* ----------------------------------------------------------------- */
		// TODO: 1) Think about resituating this statement down in the loop
		//		 2) To poll all the letters in the input input buffer and then
		//			read them from an array of buffer to save resources
		/* ----------------------------------------------------------------- */

		poll_success = poll(poll_serial, num_fds, -1);  //-1 infinite timeout, if 0, returns immediately

		if (poll_success < 0) {
			_serial_unix.print_error("Error polling input buffer");
		}

		// TODO: study if I should remove this statement
		else if (poll_success > 0) {

			/*	 --- for debugging ------*/
			//cout << "consuming...\n";     
			//cout << received;

			if (poll_serial->revents != 0) {

				/*cout << (poll_serial_r->revents & POLLIN) ? "POLLIN " : "";
				cout << (poll_serial_r->revents & POLLHUP) ? "POLLHUP " : "";
				cout << (poll_serial_r->revents & POLLERR) ? "POLLERR " : "";
				*/
				//cout << "here";
				if (poll_serial->revents & POLLIN) {
					// read one character at a time with read function
					received = ::read(serial_handle,
						&buffer,
						sizeof(buffer));

					// returns -1 if there's an error
					if (received < 0) {
						_serial_unix.print_error("Error reading pattern");

					}

					// check if found the starts_with character
					if (buffer != starts_with && !pat_begin) {
						continue;
					}
					else {
						// found the first character
						pat_begin = true;
					}

					// store the character in a string --- appending every letter, makes it slow ----
					SerialBuffer.append(1, buffer);

					/* SERIOUS bug when no polling is there needs to be looked at, for some reason, it only prints counter when you print buffer*/
					/*cout << buffer;
					cout << 's' << endl;
					cout << counter << endl;*/

					// increase counter for every ends_with character
					if (buffer == ends_with) {
						counter++;

						//Add new line between patterns? TODO: Problem whem removing this line
						//SerialBuffer.append(1, '\n');

						// reset pattern
						pat_begin = false;
					}
				}
				else {
					std::clog << "poll error revents" << std::endl;
					exit(1);
				}

			}
			else {
				std::clog << "poll error" << std::endl;
				exit(1);
			}
		}

		// while counter is smaller than the instances
	} while (counter < num_instances);

	return SerialBuffer;

#elif defined (_WIN32)
	//win32 code here
	return ""
#endif
}


/*
/ DESCRIPTION;
/ stops reading when it find new line character '\n'
/
/ ALTERNATIVLY:
/ to use Canonical mode
/
/ RETURN:
/  one-line in a string instance
/
/ TODO:
/ Add a time-out in case there weren't any new-line character '\n'
*/

string Serial_Module::read_line() {
#if defined (__unix__)

	serial_handle = _serial_unix.serial_handle();
	poll_serial->fd = serial_handle;
	poll_serial->events |= POLLIN;
	num_fds = 1;


	// allocate memeory for read buffer
	char char_buffer;

	// allocate memeory for read buffer
	string string_buffer = "";

	// allocating variables for processing the buffer
	int received;

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	do {

		// poll event waiting for input
		poll_success = poll(poll_serial, num_fds, -1);  //-1 return immediately, no timeouts

		if (poll_success < 0) {
			_serial_unix.print_error("Error polling input buffer");
		}

		else if (poll_success > 0) {

			if (poll_serial->revents != 0) {

				if (poll_serial->revents & POLLIN) {


					// read one character at a time with read function
					received = ::read(_serial_com,
						&char_buffer,				// Reference to char_buffer
						sizeof(char_buffer));		// size of char_buffer


					// returns -1 if there's an error
					if (received < 0) {
						_serial_unix.print_error("Error reading line");
					}

					string_buffer.append(1, char_buffer);

				}
				else {
					std::clog << "poll error revents" << std::endl;
					exit(1);
				}

			}
			else {
				std::clog << "poll error" << std::endl;
				exit(1);
			}
		}

		//TODO: to check for end of file/console read or add a time-out
	} while (char_buffer != '\n');

	return string_buffer;

#elif defined (_WIN32)
	//win32 code here

#endif
	return {};
}

string Serial_Module::read_line(unsigned int num_lines) {
#if defined (__unix__)

	serial_handle = _serial_unix.serial_handle();
	poll_serial->fd = serial_handle;
	poll_serial->events |= POLLIN;
	num_fds = 1;

	// allocate memeory for read buffer
	char char_buffer;

	// allocate memeory for read buffer
	string string_buffer = "";

	// allocating variables for processing the buffer
	int received;

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	do {

		// poll event waiting for input
		poll_success = poll(poll_serial, num_fds, -1);  //-1 return immediately, no timeouts

		if (poll_success < 0) {
			_serial_unix.print_error("Error polling input buffer");
		}

		else if (poll_success > 0) {

			if (poll_serial->revents != 0) {

				if (poll_serial->revents & POLLIN) {


					// read one character at a time with read function
					received = ::read(_serial_com,
						&char_buffer,				// Reference to char_buffer
						sizeof(char_buffer));		// size of char_buffer


					// returns -1 if there's an error
					if (received < 0) {
						_serial_unix.print_error("Error reading line");
					}

					string_buffer.append(1, char_buffer);

					//decrease number of lines if found one
					if (char_buffer == '\n')
						--num_lines;

				}
				else {
					std::clog << "poll error revents" << std::endl;
					exit(1);
				}

			}
			else {
				std::clog << "poll error" << std::endl;
				exit(1);
			}
		}

	} while (num_lines > 0);

	return string_buffer;

#elif defined (_WIN32)
	//win32 code here

#endif
	return {};
}

// -------------- WRITING METHODS -------------- //


int8_t Serial_Module::write(char const& char_to_write) {
#if defined (__unix__)

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	// convert to string to use c_str();
	std::string to_string(1, char_to_write);

	// number of bytes is one
	const short unsigned n_bytes = 1;

	_success = ::write(_serial_com, to_string.c_str(), n_bytes);

	if (_success < 0) {
		_serial_unix.print_error("Error writing character");
	}

	return 1;

#elif defined (_WIN32)
	//win32 code here

#endif
	return {};
}

int8_t Serial_Module::write(string const& str_to_write) {
#if defined (__unix__)

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	// convert to string to use c_str();
	std::string string_to_wrt = str_to_write;

	// number of bytes is length of string
	const short unsigned n_bytes = string_to_wrt.length();

	_success = ::write(_serial_com, string_to_wrt.c_str(), n_bytes);

	if (_success < 0) {
		_serial_unix.print_error("Error writing string");
	}

	return 1;

#elif defined (_WIN32)
	//win32 code here

#endif
	return {};
}


//------------------ Control Signals ------------------//


// ----------------- Output Signals ----------------//

int8_t Serial_Module::set_DTR() {

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	// to hold status of pin
	int status;

	// get DTR status
	_success = ::ioctl(_serial_com, TIOCMGET, &status);
	if (_success < 0) {
		_serial_unix.print_error("Error getting DTR status");
		return -1;
	}

	// drop DTR bit
	status |= TIOCM_DTR;

	// set DTR status
	_success = ::ioctl(_serial_com, TIOCMSET, status);
	if (_success < 0) {
		_serial_unix.print_error("Error setting DTR status");
		return -1;
	}

	return 1;
}


int8_t Serial_Module::set_RTS() {

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	// to hold status of pin
	int status;

	// get RTS status
	_success = ::ioctl(_serial_com, TIOCMGET, &status);
	if (_success < 0) {
		_serial_unix.print_error("Error getting RTS status");
		return -1;
	}

	// drop RTS bit
	status |= TIOCM_RTS;

	// set RTS status
	_success = ::ioctl(_serial_com, TIOCMSET, status);
	if (_success < 0) {
		_serial_unix.print_error("Error setting RTS status");
		return -1;
	}

	return 1;
}


int8_t Serial_Module::drop_DTR() {

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	// to hold status of pin
	int status;

	// get DTR status
	_success = ::ioctl(_serial_com, TIOCMGET, &status);
	if (_success < 0) {
		_serial_unix.print_error("Error getting DTR status");
		return -1;
	}

	// drop DTR bit
	status &= ~TIOCM_DTR;

	// set DTR status
	_success = ::ioctl(_serial_com, TIOCMSET, status);
	if (_success < 0) {
		_serial_unix.print_error("Error setting DTR status");
		return -1;
	}

	return 1;
}


int8_t Serial_Module::drop_RTS() {

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	// to hold status of pin
	int status;

	// get RTS status
	_success = ::ioctl(_serial_com, TIOCMGET, &status);
	if (_success < 0) {
		_serial_unix.print_error("Error getting RTS status");
		return -1;
	}

	// drop DTR bit
	status &= ~TIOCM_RTS;

	// set RTS status
	_success = ::ioctl(_serial_com, TIOCMSET, status);
	if (_success < 0) {
		_serial_unix.print_error("Error setting RTS status");
		return -1;
	}

	return 1;
}

// ------------------- Input/Output Signals --------------------//

int8_t Serial_Module::DSR_status() {

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	// to hold status of pin
	int status;

	// get DSR status
	_success = ::ioctl(_serial_com, TIOCMGET, &status);
	if (_success < 0) {
		_serial_unix.print_error("Error getting DSR status");
		return -1;
	}

	// get status
	status &= TIOCM_DSR;

	if (status == 1) return 1;
	else return 0;
}


int8_t Serial_Module::DTR_status() {
	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	// to hold status of pin
	int status;

	// get DTR status
	_success = ::ioctl(_serial_com, TIOCMGET, &status);
	if (_success < 0) {
		_serial_unix.print_error("Error getting DTR status");
		return -1;
	}

	// get status
	status &= TIOCM_DTR;

	if (status == 1) return 1;
	else return 0;
}


int8_t Serial_Module::RTS_status() {

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	// to hold status of pin
	int status;

	// get RTS status
	_success = ::ioctl(_serial_com, TIOCMGET, &status);
	if (_success < 0) {
		_serial_unix.print_error("Error getting RTS status");
		return -1;
	}

	// get status
	status &= TIOCM_RTS;

	if (status == 1) return 1;
	else return 0;
}


int8_t Serial_Module::CTS_status() {

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	// to hold status of pin
	int status;

	// get CTS status
	_success = ::ioctl(_serial_com, TIOCMGET, &status);
	if (_success < 0) {
		_serial_unix.print_error("Error getting CTS status");
		return -1;
	}

	// get status
	status &= TIOCM_CTS;

	if (status == 1) return 1;
	else return 0;
}


int8_t Serial_Module::DCD_status() {

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	// to hold status of pin
	int status;

	// get DCD status
	_success = ::ioctl(_serial_com, TIOCMGET, &status);
	if (_success < 0) {
		_serial_unix.print_error("Error getting DCD status");
		return -1;
	}

	// get status
	status &= TIOCM_CAR;

	if (status == 1) return 1;
	else return 0;
}


int8_t Serial_Module::RI_status() {

	// handle to serial_unix
	int _serial_com = _serial_unix.serial_handle();

	// to hold status of pin
	int status;

	// get RI status
	_success = ::ioctl(_serial_com, TIOCMGET, &status);
	if (_success < 0) {
		_serial_unix.print_error("Error getting RI status");
		return -1;
	}

	// get status
	status &= TIOCM_RI;

	if (status == 1) return 1;
	else return 0;
}


bool Serial_Module::set_config(Serial_Config const& config) {

	if (config.is_init == true) {
		_serial_unix.set_config_unix(config);
		return 1;
	}
	else {
		cerr << "Error: Call get_config first.";
		return 0;
	}
}

bool Serial_Module::get_config(Serial_Config& config) {

	config = _serial_unix.get_config_unix();

	return 1;
}


bool Serial_Module::close() {
#if defined(__unix__)

	//deallocate heap memory
	free(poll_serial);

	_success = this->_serial_unix.serial_close();
	return _success;

#elif defined (_WIN32)
	//win32 code here

#endif
	return 0;
}

Serial_Module::~Serial_Module() {}