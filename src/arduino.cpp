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

#include "arduino.h"

// Serial ports are complicated
#include <unistd.h>
#include <cstdio>
#include <cstdlib> 
#include <cstdint>
#include <cstring>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <termios.h>
#include <signal.h>
#include <setjmp.h>
#include <sys/ioctl.h>
#include <sys/types.h>

using namespace std;

arduino::arduino(string port, int num_devices, string key) : port(port), device(num_devices, key){
	status = (int *) calloc(num_devices, sizeof(int));
}

arduino::~arduino(){
	close(arduinofd);
	
	delete status;
}

int arduino::connect(){
	struct termios toptions;

	arduinofd = open(port.c_str(), O_RDWR | O_NOCTTY | O_NDELAY);
	if (arduinofd == -1){
		return 1;
	}
	
	if(tcgetattr(arduinofd, &toptions) < 0){
		return 2;
	}
	speed_t brate = B9600; //Baud rate to 9600
	cfsetispeed(&toptions, brate);
	cfsetospeed(&toptions, brate);
	
	toptions.c_cflag &= ~PARENB;
	toptions.c_cflag &= ~CSTOPB;
	toptions.c_cflag &= ~CSIZE;
	toptions.c_cflag |= CS8;
	toptions.c_cflag &= ~CRTSCTS;
	toptions.c_cflag |= CREAD | CLOCAL;
	toptions.c_iflag &= ~(IXON | IXOFF | IXANY);
	toptions.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
	toptions.c_oflag &= ~OPOST;

	toptions.c_cc[VMIN]  = 0;
	toptions.c_cc[VTIME] = 20;
	
	if(tcsetattr(arduinofd, TCSANOW, &toptions) < 0){
		return 4;
	}

	return 0;
}

string arduino::process(string data){
	data += ";"; // Arduino messages ';' terminated (but the server does not know this)
	if(write(arduinofd, data.c_str(), data.length()) != data.length()){
		return "could not access arduino";
	}

	// Status will always be returned by the Arduino
	string response = "";
	bool reading = true;
	while(reading){
		char respcstr[8];
		bzero(respcstr, 8);
		int size = read(arduinofd, respcstr, 8);
		reading = !(size < 8);
		for(int i = 0; i < size; i++){
			if(respcstr[i] == '\n') break;
			response += respcstr[i];
		}
	}
	
	return response;
}
