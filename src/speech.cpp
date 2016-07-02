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

#include <iostream>
#include <chrono>
#include "speech.h"
#include "ct_time.h"

using namespace std;

speech::~speech(){
	detect.clean();
	action.clean();
}

int speech::init(){
	int rv = 0;

	string libdir = getparam("speech_lib_dir", config, "lib/");
	
	rv = detect.init(getparam("speech_detect_dict", config, "lib/leplm/detect.dic"), libdir, false, false);
	rv += 10*action.init(getparam("speech_action_dict", config, "lib/leplm/action.dic"), libdir, false, false);
	
	detect.pause();
	action.pause();
	
	return rv;
}

string speech::recognize(int mode){
	
	string word = "NULL";
	
	if(mode == 0){
		detect.resume();
		word = detect.getword(5);
		detect.pause();
	}
	else if(mode == 1){
		action.resume();
		word = action.getword(4);
		action.pause();
	}
	
	return word;
}

int speech::run(){
	
	detect.resume();
	bool running = true;
	
	int mode = 0;
	bool mute = false;
	while(running){
		
		if(socketsendrecv("c;check_mute") == "mute") mute = true;
		else mute = false;
		
		if(!mute){
			if(mode == 0) socketsendrecv("a;a");
			else if(mode == 1) socketsendrecv("a;b");
		}
		
		if(mute){
			csleep(1);
		}
		else{
			string word = recognize(mode);
			if(word == "NULL" || word[0] == 'e') cout << "Speech Error: " << word << "\n";
			socketsendrecv("a;d");

			bool commanded = false;
			for(int i = 0; i < config.size(); i++){
				if(config[i].param == "speech_word_"+word){
					string command = config[i].value;
					commanded = true;
					if(command == "detect") mode = 0;
					else if(command == "action") mode = 1;
					else if(command != "") socketsendrecv(command);
					else commanded = false;
				}
			}
			if(!commanded) cout << "No command registered for: " << word << "\n";
		}
	}
	
	return 0;
}
