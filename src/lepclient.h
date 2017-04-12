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

#pragma once

#include "lepmodule.h"

#include <ctclientsecure.h>

struct serverdata {
	std::string ip;
	int port;
};

class lepclient : public lepmodule, private ctclientsecure {
	
public:
	lepclient(std::string password, std::string key, std::vector<configtuple> params);
	
protected:
	
	std::string get_time_password();
	std::string socketsendrecv(std::string message);

	serverdata server;
	std::vector<configtuple> config;
};
