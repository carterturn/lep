/*
  Copyright 2016 Carter Turnbaugh

  This file is part of LEP.

  LEP is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  LEP is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with LEP.  If not, see <http://www.gnu.org/licenses/>.
*/

#include "raspberry_pi.h"
#include "util.h"

#include <cstdlib>
#include <sstream>
#include <unistd.h>

using namespace std;

raspberry_pi::raspberry_pi(vector<string> parameters) : device(parameters.size() - 2, parameters[0]){
	for(int i = 2; i < parameters.size(); i++){
		pins.push_back(atoi(parameters[i].c_str()));
	}
	status = new int[pins.size()];
}

raspberry_pi::~raspberry_pi(){
	// Unexport files

	for(int i = 0; i < num_devices; i++){
		fstream unexport_file;
		unexport_file.open("/sys/class/gpio/unexport");
		unexport_file << pins[i];
		unexport_file.close();
	}

	delete status;
}

int raspberry_pi::connect(){

	// Export files
	for(int i = 0; i < num_devices; i++){
		fstream export_file;
		export_file.open("/sys/class/gpio/export");
		export_file << pins[i];
		export_file.close();
	}

	// Set files to output
	for(int i = 0; i < num_devices; i++){
		stringstream filename;
		filename << "/sys/class/gpio/gpio" << pins[i] << "/direction";
		fstream direction_file(filename.str().c_str());
		direction_file << "out";
		direction_file.close();
	}
	
	usleep(1000000);

	// Export files
	for(int i = 0; i < num_devices; i++){
		fstream export_file;
		export_file.open("/sys/class/gpio/export");
		export_file << pins[i];
		export_file.close();
	}

	// Set files to output
	for(int i = 0; i < num_devices; i++){
		stringstream filename;
		filename << "/sys/class/gpio/gpio" << pins[i] << "/direction";
		fstream direction_file(filename.str().c_str());
		direction_file << "out";
		direction_file.close();
	}

	return 0;
}

string raspberry_pi::process(string data){

	vector<string> command = split_string(data, ',');

	int device_num = atoi(command[0].c_str());

	if(command[0] == "status"){
		device_num = -1;
	}
	else if(device_num > num_devices - 1 || device_num < 0){
		return "device not found";
	}
	else{
		stringstream filename;
		filename << "/sys/class/gpio/gpio" << pins[device_num] << "/value";
		fstream pin_file;
		pin_file.open(filename.str().c_str());
		
		if(command.size() > 1){
			pin_file << command[1];
		}
		else{
			string current;
			getline(pin_file, current);
			pin_file.close();
			pin_file.open(filename.str().c_str());			
			if(current == "0"){
				pin_file << "1";
			}
			else {
				pin_file << "0";
			}
		}

		pin_file.close();
	}
	// Status will always be returned by the Raspberry Pi
	stringstream response;
	for(int i = 0; i < num_devices; i++){
		stringstream filename;
		filename << "/sys/class/gpio/gpio" << pins[i] << "/value";
		fstream pin_file;
		pin_file.open(filename.str().c_str());
		
		string current;
		getline(pin_file, current);
		response << current;
		if(i < num_devices - 1) response << ",";

		pin_file.close();
	}
	
	return response.str();
}
