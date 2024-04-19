#if defined (__unix__) 
#include "Serial_Unix.h"

#elif defined (_WIN32)
#include "Serial_Win.h"

#endif

#ifndef Serial_Module_H
#define Serial_Module_H


// ----------------- START OF SERIAL_MODULE CLASS DECLARATION -------------------//


class Serial_Module {


	// -------------- PRIVATE MEMBERS --------------//
private :
#if defined (__unix__)
	Serial_Unix _serial_unix;
	struct pollfd* poll_serial;

	int num_fds;
	int poll_success;
	int serial_handle;

#elif defined (_WIN32)
	Serial_Handle _serial_win;

#endif

	bool _success;

	// --------------- PUBLIC MEMBERS ---------------//
public:


	// initialize with no configurations
	Serial_Module();

	// optional initialize with configurations
	Serial_Module(string const& _com_num);

	// either through constructor or from here
	int8_t init(string const& _com_num);

	unsigned int bytes_available();

	// reading methods
	char read();								// reads one char
	string read(size_t num_chars);				// reads number of chars specified
	string read_pattern(char starts_with,		// reads a pattern starts with and ends with
						char ends_with, 
						unsigned int num_instances);		
	string read_line();							// reads one line in canonical mode
	string read_line(unsigned int num_lines);

	// writing methods
	int8_t write(char const& char_to_write);
	int8_t write(string const& str_to_write);

	//------------ RS-232 Control Signals ---------------//

	// --- Output Signals ---
	int8_t set_DTR();
	int8_t set_RTS();
	int8_t drop_DTR();
	int8_t drop_RTS();

	// --- Input/Output Signals ---
	int8_t DSR_status();
	int8_t DTR_status();
	int8_t RTS_status();
	int8_t CTS_status();
	int8_t DCD_status();
	int8_t RI_status();

	// ----------- Configuration method -----------//

	bool set_config(Serial_Config const& config);

	//TODO: check for errors in returning config object
	bool get_config(Serial_Config& config);

	bool close();
	~Serial_Module();
};

#endif

// ------------- END OF SERIAL_MODULE CLASS --------------//