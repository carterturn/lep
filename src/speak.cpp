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

#include "speak.h"
#include <cstring>
#include <chrono>
#include <iomanip>
#include <espeak/speak_lib.h>

using namespace std; //Yes, this is longer than writing std::string words on line 31. And yes, this comment is even longer. And yes, I wrote this on the day when I was reading funny comments on the Internet.

speak::speak(string key) : action(key){
    espeak_Initialize(AUDIO_OUTPUT_PLAYBACK, 0, NULL, 0);
}

speak::~speak(){    
    while(espeak_Terminate() != 0) cout << "Espeak terminate error\n";
}

string speak::process(string words){
    
    time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
    int hour = localtime(&now)->tm_hour;
    
    espeak_SetVoiceByName("en+12");
    
    if(hour > 11 && hour < 18){
        espeak_SetParameter(espeakRATE, 230, 0);
        espeak_SetParameter(espeakRANGE, 90, 0);
    }
    else if(hour > 8 && hour < 22){
        espeak_SetParameter(espeakRATE, 210, 0);
        espeak_SetParameter(espeakRANGE, 85, 0);
    }
    else{
        espeak_SetParameter(espeakRATE, 170, 0);
        espeak_SetParameter(espeakRANGE, 75, 0);
    }
    if(hour > 7 && hour < 22) espeak_SetParameter(espeakVOLUME, 200, 0);
    else espeak_SetParameter(espeakVOLUME, 100, 0);
    espeak_SetParameter(espeakPITCH, 73, 0); //Set pitch and range to 1 to reduce warmth
    espeak_SetParameter(espeakPUNCTUATION, 10, 0);
    espeak_SetParameter(espeakCAPITALS, 1, 0);
    espeak_SetParameter(espeakWORDGAP, 1, 0);
    
    espeak_Synth(words.c_str(), strlen(words.c_str())+1, 0, POS_CHARACTER, 0, espeakCHARS_AUTO, NULL, NULL);
    espeak_Synchronize();
    
    return "ok";
}
