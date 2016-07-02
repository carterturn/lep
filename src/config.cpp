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

#include "config.h"
#include <fstream>
#include <regex>
#include <cstdlib>
#include <ctlog.h>

// Different module types
#include "lepserver.h"
#include "speech.h"
#include "gui.h"
#include "text.h"

using namespace std;

lepmodule * getmodule(string configpath){
	
	fstream configfile;
	configfile.open(configpath);
	
	vector<configtuple> config;
	
	regex trimmer("(^[ \t]*)(.*[^ \t])([\t ]*$)");
	regex tupledetect("^([^=]*)([ \t=]*)([^=]*)$");
	
	while(!configfile.eof()){
		string line;
		getline(configfile, line);
		
		line = regex_replace(line, trimmer, "$2");
		
		if(!(line.substr(0, 2) == "//") && line != ""){
			configtuple parameter;
			parameter.param = regex_replace(regex_replace(line, tupledetect, "$1"), trimmer, "$2");
			parameter.value = regex_replace(regex_replace(line, tupledetect, "$3"), trimmer, "$2");

			cout << parameter.param << " set to " << parameter.value << "\n";
			config.push_back(parameter);
		}
	}
	
	
	configfile.close();
	
	moduledata data;
	
	vector<configtuple> moduleconfig;
	
	for(int i = 0; i < config.size(); i++){
		if(config[i].param == "type"){
			data.type = config[i].value;
		}
		else if(config[i].param == "password"){
			data.password = config[i].value;
		}
		else if(config[i].param == "key"){
			data.key = config[i].value;
		}
		else{
			moduleconfig.push_back(config[i]);
		}
	}
	
	lepmodule *newmodule;
	
	if(data.type == "server"){
		newmodule = new lepserver(data.password, data.key, moduleconfig);
	}
	else if(data.type == "speech"){
		newmodule = new speech(data.password, data.key, moduleconfig);
	}
	else if(data.type == "gui"){
		newmodule = new gui(data.password, data.key, moduleconfig);
	}
	else if(data.type == "text"){
		newmodule = new text(data.password, data.key, moduleconfig);
	}
	else {
		cerr << "Module type not found: " << data.type << "\n";
	}
	
	return newmodule;
}
