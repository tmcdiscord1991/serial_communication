
#include "GPS_Module.h"

class test {
public:


	Serial_Module serial;
	

	enum GPS_NMEA {
		GGA,
		GSA,
		RMC,
		VTG,
		GSV,
		NMEA_LEN
	};

	string a;

	string _gps_data[NMEA_LEN];

	test(string com) : serial(com) {}

	void update() {


		string nmea_data = serial.read_pattern('$', '\n', 4);
		//	string nmea_data = serial.read_pattern('$', '\n', 4);

		size_t pos = 0;
		string deli = "\r\n";		// string delimiter
		string gps_snt;			// one gps header token

		while ((pos = nmea_data.find(deli)) != string::npos) {

			gps_snt = nmea_data.substr(0, pos);
			if (gps_snt.substr(4, 1).compare("G") == 0) 
				a = gps_snt;
			else if (gps_snt.substr(4, 2).compare("SA") == 0)
				_gps_data[GSA] = gps_snt;
			else if (gps_snt.substr(4, 1).compare("M") == 0)
				_gps_data[RMC] = gps_snt;
			else if (gps_snt.substr(4, 1).compare("T") == 0)
				_gps_data[VTG] = gps_snt;
			else if (gps_snt.substr(4, 2).compare("SV") == 0)
				_gps_data[GSV] = gps_snt;
			else
				cerr << "Error, cannot find delimiter";  //debugging

			nmea_data.erase(0, pos + deli.length());			
		}

		return;
	}

	string get_gga() {
		return a;
	}

	string get_gsa() {
		return _gps_data[GSA];
	}
	string get_rmc() {
		return _gps_data[RMC];
	}
	string get_vtg() {
		return _gps_data[VTG];
	}
};

int main() {


	Serial_Module serial("/dev/ttyS4");

	while (true) {
		//cout << "afaf";
		//cout << serial.write("hello");
		//cout << "return from read pattern";
		cout << serial.read_line(4);
	}
	
	//GPS_Module gps("/dev/ttyS4");

	/*cfg::GPS_Config config;
	gps.get_gps_config(config);
	config.SET_UPDATE = false;
	config.SET_P_THREAD = true;
	gps.set_gps_config(config);
	
	/*
	cout << a << endl;
	cout << b << endl;
	cout << c << endl;
	cout << d << endl;*/

	//string a = "$afd\r\n$acfer\r\n"; 
	//cout << a;

	/*size_t pos = 0;
	string deli = "\r\n";		// string delimiter
	string nmea_data = "$afd\r\n$acfer\r\n";			// one gps header token
	string gps_snt;
	while ((pos = nmea_data.find(deli)) != string::npos) {

		gps_snt = nmea_data.substr(0, pos);
		cout << gps_snt;

		nmea_data.erase(0, pos + deli.length());
	}*/
	/*while (true) {		
		gps.update();
		cout << gps.get_GP_GGA() << endl;
		//std::this_thread::sleep_for(std::chrono::milliseconds(1000)); // corss-platform sleep function
	}
	//cout << "error";
	gps.close();
	//serial.close();
	

	//test t("/dev/ttyS4");

	/*string a = "abc";
	cout << a;
	a = "abcd";
	cout << a;
	a = "abc";
	cout << a;*/
	
	/*while (true) {
		t.update();
		cout << t.get_gga();
		std::cout.flush();

		//cout << t.get_gsa();
		//cout << t.get_rmc();
		//cout << t.get_vtg();
		//break;
	}*/

	
	
	return 0;
}


