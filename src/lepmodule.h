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

#include <iostream>
// Will be used by subclasses
#include <vector>

struct moduledata{
	std::string type;
	std::string password;
	std::string key;
};

struct configtuple{
	std::string param;
	std::string value;
};

// Superclass for all LEP modules

class lepmodule {

public:
	lepmodule(std::string password, std::string key);
	
	// Initializer is seperate as some things only need to be done once
	virtual int init() = 0;
	
	// Run is the main loop containing a listening socket or information collection and transmission
	virtual int run() = 0;

protected:
	
	std::string getparam(std::string paramname, std::vector<configtuple> params, std::string defaultval);
	
	std::string password;
	std::string key;
};
