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

#include <iostream>
#include <chrono>
#include <algorithm>
#include "speech.h"
#include "ct_time.h"

using namespace std;

speech::~speech(){
    recognizer.clean();
}

int speech::init(){
    int rv = 0;

    rv = recognizer.init(getparam("speech_dict", config, "lib/leplm/lep.dict"),
                 getparam("speech_hmm", config, "lib/lephmm"),
                 getparam("speech_jsgf", config, "lib/leplm/lep.jsgf"));

    return rv;
}

bool is_space(char c){
    return c == ' ';
}

int speech::run(){
    bool running = true;
    
    bool mute = false;
    while(running){
        
        if(socketsendrecv("c;check_mute") == "mute") mute = true;
        else mute = false;
        
        if(!mute){
            socketsendrecv("a;b");
        }
        
        if(mute){
            csleep(1);
        }
        else{
            string word = recognizer.getword();

            socketsendrecv("a;d");
            
            replace_if(word.begin(), word.end(), is_space, '_');
            if(word == "" || word[0] == '#') cout << "Speech Error: " << word << "\n";

            bool commanded = false;
            for(int i = 0; i < config.size(); i++){
                if(config[i].param == "speech_word_"+word){
                    string command = config[i].value;
                    commanded = true;
                    if(command != "") socketsendrecv(command);
                    else commanded = false;
                }
            }
            if(!commanded) cout << "No command registered for: " << word << "\n";
        }
    }
    
    return 0;
}

int main(int argc, char* argv[]){
    string config = "lep.conf";
    
    if(argc > 1) config = argv[1];

    string password;
    string key;
    vector<configtuple> params = getconfig(config, password, key);

    speech s(password, key, params);

    int code = s.init();
    if(code != 0){
        cout << "ERROR: " << code << "\n";
        return code;
    }

    return s.run();
}
