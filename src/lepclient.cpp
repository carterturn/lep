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

#include "lepclient.h"

#include <chrono>

using namespace std;

lepclient::lepclient(string password, string key, vector<configtuple> params) : lepmodule(password, key), ctclientsecure(key){
	server.ip = getparam("server", params, "127.0.0.1");
	server.port = atoi(getparam("server_port", params, "12516").c_str());
	config = params;
}

string lepclient::get_time_password(){
	string time_password = "";
	
	int now = chrono::system_clock::now().time_since_epoch() /  chrono::seconds(1);
	for(int i = 0; i < 4; i++){
		time_password = (char) (now & 0xFF) + time_password;
		now = now >> 8;
	}

	time_password += password;
	cout << time_password << "\n";
	
	return time_password;
}

string lepclient::socketsendrecv(string message){
	if(create(server.ip, server.port) < 0){
		return "ERROR";
	}

	s_write(get_time_password());
	string response = s_read();
	if(response == "ok"){
		s_write(message);
		response = s_read();
	}
	
	c_close();
	
	return response;
}
