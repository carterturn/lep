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
#include <sstream>

#include <sys/socket.h>
#include <resolv.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <chrono>

#include "gui.h"
#include "ct_time.h"
#include "util.h"

using namespace std;

gui::gui(string password, string key, vector<configtuple> params) : lepclient(password, key, params){
    configfile = getparam("gui_config_file", params, "guiconf.txt");
    int era = atoi(getparam("gui_colors", params, "1").c_str());

    window_x = atoi(getparam("gui_x_size", params, "700").c_str());
    window_y = atoi(getparam("gui_y_size", params, "400").c_str());

    int number_multibuttons = atoi(getparam("gui_number_multibuttons", params, "0").c_str());
    int number_endmultibuttons = atoi(getparam("gui_number_endmultibuttons", params, "0").c_str());

    for(int i = 0; i < number_multibuttons; i++){
        multibutton_commands.push_back(getparam("gui_multibutton_"+its(i), params, ""));
    }
    for(int i = 0; i < number_endmultibuttons; i++){
        endmultibutton_commands.push_back(getparam("gui_endmultibutton_"+its(i), params, ""));
    }
    
    fgl = new futuregl(era);
}

gui::~gui(){
    delete fgl;
}

void click_callback(GLFWwindow * window, int button, int action, int mods){
    gui * gui_instance = (gui *) glfwGetWindowUserPointer(window);
    if(action == GLFW_PRESS){
        if(button == GLFW_MOUSE_BUTTON_LEFT){
            double mouse_x, mouse_y;
            glfwGetCursorPos(window, &mouse_x, &mouse_y);
            int window_x, window_y;
            glfwGetWindowSize(window, &window_x, &window_y);
            mouse_y = window_y - mouse_y;
            string buttonname = gui_instance->get_fgl()->getclicked((int) mouse_x, (int) mouse_y);
            if(buttonname != ""){
                gui_instance->run_button_command(buttonname);
            }

        }
    }
}

int gui::init() {
    glfwInit();
    
    window = glfwCreateWindow(window_x, window_y, "LEP", NULL, NULL);    

    if(!window){
        glfwTerminate();
        
        return -1;
    }

    // Display GUI on primary monitor (not the best default, but better than just using the far left one)
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    int position_x, position_y;

    glfwGetMonitorPos(monitor, &position_x, &position_y);
    glfwSetWindowPos(window, position_x, position_y);
    glfwSetWindowUserPointer(window, this);
    glfwSetMouseButtonCallback(window, &click_callback);
    
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetInputMode(window, GLFW_STICKY_MOUSE_BUTTONS, GL_TRUE);
    glOrtho(0, window_x, 0, window_y, -1.0, 1.0);

    fgl->load_config(configfile);
    
    return 0;
}

int gui::run(){
    cout << "Running\n";
    bool just_clicked = false; // required due to difficulties with callbacks and classes
    while(!glfwWindowShouldClose(window)){
        update_multibuttons();
        
        glClear(GL_COLOR_BUFFER_BIT);

        fgl->draw();
        
        glfwSwapBuffers(window);
        usleep(50000);
        glfwWaitEvents();
    }
    
    glfwTerminate();
    
    return 0;
}

void gui::update_multibuttons(){
    // Reduce requests by pre-caching statuses
    vector<string> statuses = split_string(socketsendrecv("c;status"), ';');
    for(int i = 0; i < multibutton_commands.size(); i++){
        vector<string> command = split_string(multibutton_commands[i], ':');
        if(command[0] == "status"){
            vector<string> desired_interface = split_string(command[1], ',');

            for(int j = 0; j < statuses.size(); j++){
                vector<string> status_parts = split_string(statuses[j], ':');

                if(desired_interface[0] == status_parts[0]){
                    int device_idx = atoi(desired_interface[1].c_str());
                    vector<string> device_statuses = split_string(status_parts[1], ',');
                    if(device_idx >= device_statuses.size()){
                        // Server did not return correctly, die
                        return;
                    }
                    fgl->setmultibutton(i, atoi(device_statuses[device_idx].c_str()));
                }
            }
        }
    }
    for(int i = 0; i < endmultibutton_commands.size(); i++){
        vector<string> command = split_string(endmultibutton_commands[i], ':');
        if(command[0] == "status"){
            vector<string> desired_interface = split_string(command[1], ',');

            for(int j = 0; j < statuses.size(); j++){
                vector<string> status_parts = split_string(statuses[j], ':');

                if(desired_interface[0] == status_parts[0]){
                    int device_idx = atoi(desired_interface[1].c_str());
                    vector<string> device_statuses = split_string(status_parts[1], ',');
                    if(device_idx >= device_statuses.size()){
                        // Server did not return correctly, die
                        return;
                    }
                    fgl->setmultibutton(i, atoi(device_statuses[device_idx].c_str()), true);
                }
            }
        }
    }
}

// Runs commands from buttons
void gui::run_button_command(string buttonname){
    string command = getparam("gui_button_"+buttonname, config, "");
    if(command != "") socketsendrecv(command);
}

futuregl * gui::get_fgl(){
    return fgl;
}

int main(int argc, char* argv[]){
    string config = "lep.conf";
    
    if(argc > 1) config = argv[1];

    string password;
    string key;
    vector<configtuple> params = getconfig(config, password, key);

    gui g(password, key, params);

    int code = g.init();
    if(code != 0){
        cout << "ERROR: " << code << "\n";
        return code;
    }

    return g.run();
}
