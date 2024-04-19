/*
/ MODULE COMMUNICATION HEADER FILE FORE UNIX
/ NOTES :
/ 1) Variables being with underscor _ are private members of the class
*/

#if defined (__unix__)

#ifndef Serial_Unix_H
#define Serial_Unix_H

#include <string.h>
#include<iostream>
#include<stdio.h>
#include <string>

#include <fcntl.h> 
#include <poll.h>

#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/select.h>
#include <sys/time.h>

#include <errno.h> 
#include <termios.h> 
#include <unistd.h> 
#include <asm/socket.h>

#include "Configuration.h" // for configurating the serial port

#include <chrono> // for timo-outs managements

// ----- Special Type Definitions ------- //

using namespace std;
using namespace cfg;

typedef short INT_16;
typedef int INT_32;

typedef signed char INT_8;
typedef unsigned char U_INT8;

typedef unsigned short U_INT16;
typedef unsigned int U_INT32;

// -------------------- Serial Unix Class-----------------------//

class Serial_Unix {

private:

	Serial_Config config_unix;

	int _serial_com;
	int _baud_rate;

	// 1 byte success flag
	int8_t _success;

	// Creating new termios struct
	struct termios serial_config;
	struct termios serial_config_old; // to store old termios attributes
	const char* _COM_num;

public:

	Serial_Unix();
	int8_t serial_init(string COM_num, int _baud_rate);
	int8_t set_COM_timeouts();
	int8_t serial_param_init();
	void config_canon();
	int8_t reset_term_set();
	int8_t flush_buffer(); // flush input/output buffer

	int8_t set_tc_attributes();
	int8_t get_tc_attributes();

	string read_line_dbg();		// for debugging
	string read_line_gps();		// for gps debugging

	bool set_config_unix(Serial_Config const& config);
	Serial_Config& get_config_unix();

	int8_t serial_close();
	int serial_handle();
	void print_error(string context);
	~Serial_Unix();

};

#endif
#endif