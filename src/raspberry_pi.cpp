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
#include <vector>

using namespace std;

raspberry_pi::raspberry_pi(int * gpio_pins, int num_devices, string key) : device(num_devices, key){
	status = (int *) calloc(num_devices, sizeof(int));
	gpio = (gpio_pin *) calloc(num_devices, sizeof(gpio_pin));
	for(int i = 0; i < num_devices; i++){
		gpio[i].pin = gpio_pins[i];
	}
}

raspberry_pi::~raspberry_pi(){

	// Close value files
	for(int i = 0; i < num_devices; i++){
		gpio[i].file.close();
	}

	delete gpio;
	delete status;
}

int raspberry_pi::connect(){

	// Export files
	fstream export_file("/sys/class/gpio/export");

	for(int i = 0; i < num_devices; i++){
		export_file << gpio[i].pin;
	}
	export_file.close();

	// Set files to output
	for(int i = 0; i < num_devices; i++){
		stringstream filename;
		filename << "/sys/class/gpio/gpio" << gpio[i].pin << "/direction";
		fstream direction_file(filename.str().c_str());
		direction_file << "out";
		direction_file.close();
	}

	// Open value files
	for(int i = 0; i < num_devices; i++){
		stringstream filename;
		filename << "/sys/class/gpio/gpio" << gpio[i].pin << "/value";
		gpio[i].file = fstream(filename.str().c_str());
	}
	
	return 0;
}

string raspberry_pi::process(string data){

	vector<string> command = split_string(data, ',');

	int device_num = atoi(command[0].c_str());

	if(device_num > num_devices){
		return "device not found";
	}
	else{
		if(command.size() > 1){
			gpio[device_num].file << command[1];
		}
		else{
			string current;
			gpio[device_num].file >> current;
			if(current == "0"){
				gpio[device_num].file << "1";
			}
			else {
				gpio[device_num].file << "0";
			}
		}
	}
	// Status will always be returned by the Raspberry Pi
	stringstream response;
	for(int i = 0; i < num_devices; i++){
		string current;
		gpio[i].file >> current;
		response << current;
		if(i < num_devices - 1) response << ",";
	}
	
	return response.str();
}

