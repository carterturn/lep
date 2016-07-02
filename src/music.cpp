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

#include "music.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

music::music(string ip, int port, string key) : ip(ip), port(port), action(key){}

string music::process(string data){
	int error;
	int msock;
	struct sockaddr_in addr;

	if((msock = socket(AF_INET,SOCK_STREAM,0))<0){
	}

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	inet_aton(ip.c_str(), &addr.sin_addr);

	error = connect(msock,(sockaddr*)&addr,sizeof(addr));
	if(error!=0){
	}
	
	data += "\n";
	
	if(data.length() > 8){
		bool done = true;
		while(done){	
			string temp = "";
			for(int i = 0; i < 8; i++){
				temp += data[0];
				if(data[0] == '\n'){
					done = false;
					i = 8;
				}
				data.erase(0,1);
			}
			send(msock,temp.c_str(),sizeof(data.c_str()),0);
		}
	}
	else{
		send(msock,data.c_str(),sizeof(data.c_str()),0);
	}
	
	shutdown(msock, 2);
	close(msock);
	
	return "ok";
}
