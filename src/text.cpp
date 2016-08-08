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

#include "text.h"

#include <cstdlib>
#include <iostream>

using namespace std;

int text::init(){
	// Really no way for this to fail
	return 0;
}

int text::run(){
	
	string message = "";
	cout << "Please enter command to send to LEP\nEnter 'quit' to close\n\n>>";
	getline(cin, message);
	while(message != "quit"){
		cout << socketsendrecv(message) << "\n";
		cout << ">>";
		getline(cin, message);
	}
	
}

int main(int argc, char* argv[]){
	string config = "lep.conf";
	
	if(argc > 1) config = argv[1];

	string password;
	string key;
	vector<configtuple> params = getconfig(config, password, key);

	text t(password, key, params);

	int code = t.init();
	if(code != 0){
		cout << "ERROR: " << code << "\n";
		return code;
	}

	return t.run();
}
