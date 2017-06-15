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

#include "music.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

using namespace std;

music::music(vector<string> parameters) :
    address(parameters[2]), port(atoi(parameters[3].c_str())), telnet_password(parameters[4] + "\n"),
    action(parameters[0]){}

string music::process(string data){
    int error;
    int music_socket;
    struct sockaddr_in addr;

    if((music_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0){
        return "could not create socket to music";
    }

    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    inet_aton(address.c_str(), &addr.sin_addr);

    error = connect(music_socket, (sockaddr*)&addr ,sizeof(addr));
    if(error!=0){
        return "could not connect to music socket";
    }

    
    send(music_socket, telnet_password.c_str(), telnet_password.length(), 0);
    
    data += "\n";
    if(data.length() > 255){
        bool done = true;
        while(done){    
            string temp = "";
            for(int i = 0; i < 255; i++){
                temp += data[0];
                if(data[0] == '\n'){
                    done = false;
                    i = 255;
                }
                data.erase(0,1);
            }
            send(music_socket, temp.c_str(), data.length(), 0);
        }
    }
    else{
        send(music_socket, data.c_str(), data.length(), 0);
    }
    
    shutdown(music_socket, 2);
    close(music_socket);
    
    return "ok";
}
