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

#include "device.h"

#include <fstream>
#include <vector>

class raspberry_pi : public device{

public:
	raspberry_pi(std::vector<int> pins, std::string key);
	~raspberry_pi();

	int connect();

	std::string process(std::string data);

protected:
	std::vector<int> pins;
	int * status;
};

