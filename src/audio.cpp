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

#include "audio.h"

#include <iostream>
#include <cstdlib>

#include <pulse/error.h>

using namespace std;

pa_simple *record = NULL;

int audio::read(int16_t *buffer, int size){
	if(record == NULL){
		resume();
	}

	int error = 0;
	
	if(pa_simple_flush(record, &error) < 0){
		cerr << "pa_simple_flush error " << pa_strerror(error) << "\n";
		pa_simple_free(record);
		return -1;
	}
	
	pa_usec_t latency;
	
	if((latency = pa_simple_get_latency(record, &error)) == (pa_usec_t) -1) {
		cerr << "pa_simple_get_latency error " << pa_strerror(error) << "\n";
		pa_simple_free(record);
		return -1;
	}
	
	if(latency > 1000000){
		cerr << "Speech error. Latency: " << latency << "\n";
	}
	
	if(pa_simple_read(record, buffer, size*sizeof(int16_t), &error) < 0){
		cerr << "pa_simple_new error " << pa_strerror(error) << "\n";
		pa_simple_free(record);
		return -1;
	}
	
	return 0;
}

int audio::pause(){
	if(record != NULL){
		pa_simple_free(record);
	}
	return 0;
}

int audio::resume(){
	pa_sample_spec inss;
	
	inss.format = PA_SAMPLE_S16LE;
	inss.channels = 1;
	inss.rate = 16000;

	int error = 0;

	record = pa_simple_new(NULL, "LEP Audio", PA_STREAM_RECORD, NULL, "Record", &inss, NULL, NULL, &error);
	if(record == NULL){
		cerr << "pa_simple_new error " << pa_strerror(error) << "\n";
		return 0;
	}
	
	return 0;
}
