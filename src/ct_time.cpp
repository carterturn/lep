/*
  Copyright 2017 Carter Turnbaugh

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

#include "ct_time.h"

#include <time.h>
#include <unistd.h>
#include <cstdlib>
#include <iostream>

using namespace std;

void cusleep(int usec){
	timespec ts1, ts2;
	if(usec > 1000000){
		ts1.tv_sec = (int) usec/1000000;
		usec = usec - ts1.tv_sec*1000000;
	}
	ts1.tv_nsec = usec*1000;
	nanosleep(&ts1, &ts2);
}

void csleep(int sec){
	timespec ts1, ts2;
	ts1.tv_sec = sec;
	ts1.tv_nsec = 0;
	nanosleep(&ts1, &ts2);
}

int getHour(){ //Returns hour of day
	
	time_t rawtime;
	struct tm * timeinfo;

	time (&rawtime);
	timeinfo = localtime (&rawtime);

	string asc_time = asctime(timeinfo);
	
	int hour = atoi(asc_time.substr(11, 2).c_str());
	
	return hour;
}
