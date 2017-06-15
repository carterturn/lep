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

#include "psintf.h"
#include "ct_time.h"
#include "audio.h"

#include <unistd.h>
#include <sstream>
#include <iostream>
#include <chrono>
#include <cstring>

#include <sphinxbase/err.h>

using namespace std;

psintf::psintf(bool print, bool ps_print) : print(print), ps_print(ps_print){}

int psintf::init(string dict, string hmm, string jsgf){
    cmd_ln_t *config;
    
    if(!ps_print) config = cmd_ln_init(NULL, ps_args(), FALSE,
                      "-hmm", hmm.c_str(),
                      "-jsgf", jsgf.c_str(),
                      "-dict", dict.c_str(),
                      "-logfn", "/dev/null", NULL);
    else config = cmd_ln_init(NULL, ps_args(), FALSE,
                  "-hmm", hmm.c_str(),
                  "-jsgf", jsgf.c_str(),
                  "-dict", dict.c_str(), NULL);
    if(config == NULL) return 1;

    ps = ps_init(config);
    if(ps == NULL) return 2;

    return 0;
}

string psintf::getword(int thresh){
    int16_t * buffer = new int16_t[BUFFER_SIZE * NUMBER_BUFFERS];
    memset(buffer, 0, BUFFER_SIZE * NUMBER_BUFFERS * sizeof(int16_t));

    int current_buffer = 0;

    audio::resume();
    while(true){
        if(audio::read(buffer + current_buffer * BUFFER_SIZE, BUFFER_SIZE)){
            clean();
            return "#Microphone error";
        }

        if(ps_start_utt(ps, NULL) < 0){
            audio::pause();
            return "#Unabel to start utt";
        }

        for(int i = current_buffer; i < current_buffer + NUMBER_BUFFERS; i++){
            ps_process_raw(ps, buffer + (i % NUMBER_BUFFERS) * BUFFER_SIZE, BUFFER_SIZE, FALSE, FALSE);
        }

        ps_end_utt(ps);

        int score;
        const char * hyp_word = ps_get_hyp(ps, &score, NULL);
        if(hyp_word != NULL && score > MINIMUM_SCORE){
            if(print) cout << hyp_word << "\t" << score << "\n";
            audio::pause();
             return string(hyp_word);
        }
        current_buffer = (current_buffer + 1) % NUMBER_BUFFERS;
    }

    return "";
}

int psintf::clean(){
    ps_free(ps);
    audio::pause();
    return 0;
}
