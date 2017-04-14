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

#include "psintf.h"
#include "ct_time.h"

#include <unistd.h>
#include <sstream>
#include <iostream>
#include <chrono>

#include <sphinxbase/err.h>

using namespace std;

int psintf::init(string dict, string hmm, string jsgf, bool ps_print, bool print){
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
	
	cycles_since_calib = 100;
	resume();
	this->print = print;
	
	return 0;
}

string psintf::getword(int thresh){

	if(cycles_since_calib > 20){
		pause();
		resume();
	}
	
	int buffer_size = 4096;
	int16 buffer[buffer_size];

	int k;
	int last_timestamp;
	while((k = cont_ad_read(cont, buffer, buffer_size)) == 0){}

	if(k < 0) return "#Unable to read audio";

	last_timestamp = cont->read_ts;

	if(ps_start_utt(ps, NULL) < 0) return "#Unabel to start utt";

	ps_process_raw(ps, buffer, k, FALSE, FALSE);

	bool reading = true;
	while(reading){
		if((k = cont_ad_read(cont, buffer, buffer_size)) < 0) return "#Unable to read audio";

		if(k == 0){
			if((cont->read_ts - last_timestamp) > SILENCE_DELAY){
				reading = false;
			}
		}

		last_timestamp = cont->read_ts;
		ps_process_raw(ps, buffer, k, FALSE, FALSE);
	}

	ps_end_utt(ps);

	int score;
	const char * hyp_word = ps_get_hyp(ps, &score, NULL);
	if(hyp_word != NULL && score > MINIMUM_SCORE){
		if(print) cout << hyp_word << "\t" << score << "\n";
		return string(hyp_word);
	}
	return "";
}

int psintf::pause(){
	if(ad != NULL){
		ad_close(ad);
		if(cont != NULL){
			ad_stop_rec(ad);
			while(ad_read(ad, NULL, 4096) >= 0){}
			cont_ad_reset(cont);

			cont_ad_close(cont);
		}

	}
	return 0;
}

int psintf::resume(){
	ad = ad_open_dev(NULL, 16000);
	if(ad == NULL) return 1;
	
	int error = 0;
	cont = cont_ad_init(ad, ad_read);
	if(cont == NULL) return 10;

	error = ad_start_rec(ad);
	if(error < 0) return error * 100;

	error = cont_ad_calib(cont);
	if(error < 0) return error * 1000;

	cycles_since_calib = 0;

	return 0;
}

int psintf::clean(){
	ps_free(ps);
	return 0;
}
