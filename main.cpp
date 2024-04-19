#include "GPS_Module.h"

/*
/
/	MAIN PROGRAM : This is an application for the GPS module
/	It prints useful information parsed on the console
/
/	NOTES:
/	1- No need to call Serial_Handle instance, the GPS_Module will take care of it
*/

int main(int argc, char* argv[])
{
	// USED FOR DEBUGGING THE WIN32 API SERIAL_HANDLE CLASS:
	// -----------------------------------------------------
	/*Serial_Handle sh;
	cout << "Return values: ";
	cout << sh.serial_init("COM4", "9600");
	cout << sh.flush_file_buffer();
	cout << sh.set_COM_timeouts();
	cout << sh.DCB_init() << endl << endl;
	while (true)
		cout << sh.read_line_wc();

	sh.serial_close();*/
	//-----------------------------------------------------

	

   // Create an instance of a GPS module
	GPS_Module gps;



	// Call the initialize method and pass the COM number
	// And set the baud rate
	gps.init("/dev/ttyS4");



	// TODO : DECIDE WETHER TO USE AN UPDATE() FUNCTION TO CALL 
	// BEFORE EVERY API FUNCTION CALL, OR A SEPARATE THREAD CALL



	// DEMO 1 : Printing all NMEA headings 
	// --------------------------------------------------
	// NOTE : YOU HAVE TO MAXIMIZE THE CONSOLE WINDOW TO SEE THE WHOLE TABLE
	// --------------------------------------------------
	while (true) {
		gps.update();
		gps.print_all_par_NMEA();
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // use sleep if multi-threaded
	}



	// DEMO 2 : Printing useful gps information
	// --------------------------------------------------
	/*while (true) {
	*   gps.update();
		cout << left << setw(14) << "UTS : "  << gps.get_utc() << endl;
		cout << left << setw(14) << "Date : " << gps.get_date() << endl;
		cout << left << setw(14) << "Latitude: " << gps.get_latitude() << endl;
		cout << left << setw(14) << "Longitude: " << gps.get_longitude() << endl;
		cout << left << setw(14) << "Speed: " << gps.get_speed() << endl;
		cout << left << setw(14) << "Num of SATs: " << gps.get_num_sat() << endl << endl;
		cout << "----------------------------" << endl;
		std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // corss-platform sleep function
	}*/



	// DEMO 3 : Printing parsed NMEA heading everyone each
	// ---------------------------------------------------
	/*while (true) {
	* 	gps.update();
		gps.print_par_GGA();
		gps.print_par_GSA();
		gps.print_par_RMC();
		gps.print_par_VTG();
		gps.print_par_GSV();
		std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // corss-platform sleep function
	}*/



	// DEMO 4 : Printing raw NMEA sentences from gps
	// ---------------------------------------------------
	/*while (true) {
		gps.update();
		cout << gps.get_GP_GGA() << endl;
		cout << gps.get_GP_GSA() << endl;
		cout << gps.get_GP_RMC() << endl;
		cout << gps.get_GP_VTG() << endl;
		cout << gps.get_GP_GSV() << endl << endl;
	}*/

	
	// Exit main with return zero
	return 0;
}

/*
/	END OF MAIN
*/







