/*
/
/-----------------------------------THIS MODULE IS OUT-OF-DATE------------------------------------
/
/
/ MODULE COMMUNICATION HEADER FILE
/ NOTES :
/ 1) Variables being with underscor _ are private members of the class
*/

#if defined (_WIN32)

#ifndef Serial_Win_H
#define Serial_Win_H

#include<windows.h>
#include<iostream>
#include<stdio.h>
#include <string>

using namespace std;

// type definitions for ease of use
typedef short INT_16;
typedef int INT_32;

typedef signed char INT_8;
typedef unsigned char U_INT8;

typedef unsigned short U_INT16;
typedef unsigned int U_INT32;

class Serial_Handle {

private:
	HANDLE _com_handle;
	DCB _dcb_params; // Device/Data Controle Block to configure controle settings for serial communication
	COMMTIMEOUTS _timeouts;
	INT_16 _success;
	LPCSTR _COM_num;
	int _baud_rate;

	Serial_Config config_unix;

public:

	// public methods for the serial handle class
	Serial_Handle();
	INT_16 serial_init(string COM_num, int _baud_rate);
	INT_16 flush_file_buffer();
	INT_16 set_COM_timeouts();
	INT_16 DCB_init();
	INT_16 event_driven_read();
	string read_line();
	string read_line_wc();
	INT_16 serial_close();
	void print_error(const char* context);
	~Serial_Handle();

};

#endif

#endif