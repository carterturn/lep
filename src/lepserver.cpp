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

#include "compile_config.h"
#include "action.h"

#ifdef SERVER_DEVICES
#include "arduino.h"
#endif

#ifdef SERVER_AUDIO
#include "music.h"
#endif

#include "speak.h"
#include "util.h"

#include "config.h"

#include "lepserver.h"

#include <thread>
#include <string>
#include <iostream>
#include <unistd.h>
#include <chrono>
#include <cstdlib>
#include <vector>
using namespace std;

//12516 is LEP

lepserver::lepserver(std::string password, std::string key, vector<configtuple> params) : lepmodule(password, key), ctserversecure(key) {
	
	for(int i = 0; i < params.size(); i++){
		if(params[i].param == "server_action"){

			vector<string> action_values = split_string(params[i].value, ',');

#ifdef SERVER_DEVICES
			if(action_values[1] == "arduino"){
				// Default values
				string arduino_port = action_values[2];
				int arduino_devices = atoi(action_values[3].c_str());

				device * a = new arduino(arduino_port, arduino_devices, action_values[0]);
				actions.push_back(a);
				devices.push_back(a);
			}
#endif
#ifdef SERVER_AUDIO
			if(action_values[1] == "music"){
				// Default values
				string music_ip = action_values[2];
				int music_port = atoi(action_values[3].c_str());
				
				actions.push_back(new music(music_ip, music_port, action_values[0]));
			}
			else if(action_values[1] == "speak"){
				actions.push_back(new speak(action_values[0]));
			}
#endif
		}
		else if(params[i].param == "server_port"){
			port = atoi(params[i].value.c_str());
		}
	}
}

lepserver::~lepserver(){
	for(int i = 0; i < actions.size(); i++){
		delete actions[i];
	}
}

int lepserver::checktimepass(){
	string timepass = s_read();
	int times[4];
	int shift = 0;
	for(int i = 0; i < 4; i++){
		times[i] = ((timepass[i] ^ password[i % password.length()]) & 0x000000ff) << shift;
		shift = (shift + 8) % 32;
	}
	for(int i = 4; i < timepass.length(); i++){
		if(times[i % 4] != ((timepass[i] ^ password[i % password.length()]) & 0x000000ff) << shift) times[i % 4] = 0;
		shift = (shift + 8) % 32;
	}
	if(timepass.length() != password.length()) return false;
	return abs(times[0]+times[1]+times[2]+times[3] - (chrono::system_clock::now().time_since_epoch() / chrono::minutes(1)));
}

int lepserver::init(){
	create(port);

	// Actually connect to external devices
	for(int i = 0; i < devices.size(); i++){
		devices[i]->connect();
	}
}

int lepserver::run(){
	while(true){
		
		// Wait for incomming message
		getconn();
		
		int timedelta = checktimepass();
		if(timedelta < 2){ // Verify password
			s_write("ok");
			// Get command
			vector<string> command = split_string(s_read(), ';');

			if(command.size() != 2){
				s_write("malformed command");
			}
			else{
				// "Core" (server data) commands begin with c
				if(command[0] == "c"){
				
					if(command[1] == "reboot"){
						s_write("goodbye");
						c_close();
						return 0;
					}

					s_write(process(command[1]));
				}
				// All other commands are handled by an action
				else {
					cout << command[0] << " " << command[1] << "\n";
					for(int i = 0; i < actions.size(); i++){
						if(command[0] == actions[i]->getkey()){
							actions[i]->process(command[1]);
							s_write("finished");
							break;
						}
					}
				}
			}
			
		}
		else{
			cout << "denied: " << timedelta << "\n";
			s_write("denied");
		}
		
		c_close();
	}
	
	// This should never happen (but it won't really be a problem if it does)
	return 0;
}

string lepserver::process(string command){
	if(command == "status"){ // Allows other modules to know the status of the devices
		string device_status_str = "";
					
		for(int i = 0; i < devices.size(); i++){
			device_status_str += devices[i]->getkey() + ":" + devices[i]->process("status") + ";";
		}
					
		return device_status_str;
	}
	else if(command == "check_mute"){ // Coordination for muting a speech module
		if(mute_speech){
			return "mute";
		}
		else{
			return "nomute"; // Yes, backwards compatible (like anything else is)
		}
	}
	else if(command == "mute"){
		mute_speech = true;
		return "finished";
	}
	else if(command == "unmute"){
		mute_speech = false;
		return "finished";
	}
}

int main(int argc, char* argv[]){
	string config = "lep.conf";
	
	if(argc > 1) config = argv[1];

	string password;
	string key;
	vector<configtuple> params = getconfig(config, password, key);

	lepserver server(password, key, params);

	int code = server.init();
	if(code != 0){
		cout << "ERROR: " << code << "\n";
		return code;
	}

	return server.run();
}
