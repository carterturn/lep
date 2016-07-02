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

using namespace std;

int psintf::init(string dict, string libdir, bool vps, bool v){
	cmd_ln_t *config;
	
	if(!vps) config = cmd_ln_init(NULL, ps_args(), FALSE,
				      "-hmm", string(libdir + "/lephmm").c_str(),
				      "-lm", string(libdir + "/leplm/lep.lm.DMP").c_str(),
				      "-dict", dict.c_str(),
				      "-logfn", "/dev/null", NULL);
	else config = cmd_ln_init(NULL, ps_args(), FALSE,
				  "-hmm", string(libdir + "/lephmm").c_str(),
				  "-lm", string(libdir + "/leplm/lep.lm.DMP").c_str(),
				  "-dict", dict.c_str(), NULL);
	if(config == NULL) return 1;
	
	ps = ps_init(config);
	if(ps == NULL) return 2;
	
	verb = v;
	
	return 0;
}

string psintf::getword(int thresh){

	const int smallsamplesize = SAMPLE_SIZE / SUB_BUFFERS;
	
	string words[WORD_CACHE_SIZE];
	for(int i = 0; i < WORD_CACHE_SIZE; i++) words[i] = "";
	
	int worditer = 0;
	bool detected = false;
	int rv; // return value for pocketsphinx functions

	int16_t buffer[SAMPLE_SIZE] = {0};
	int index = SUB_BUFFERS - 1;
	
	arw.read(buffer, SAMPLE_SIZE);
	
	while(!detected){

		if(arw.read(&buffer[index*smallsamplesize], smallsamplesize) < 0) return "ERROR";
		// Time loop to increase precision
		chrono::steady_clock::time_point start = chrono::steady_clock::now();
		
		index = (index + 1) % SUB_BUFFERS;

	        rv = ps_start_utt(ps, NULL);
		
		for(int i = index; i < SUB_BUFFERS + index; i++){
			int16_t *tmpbuf;
			
			tmpbuf = &buffer[(i%SUB_BUFFERS)*smallsamplesize];
			
			rv = ps_process_raw(ps, tmpbuf, smallsamplesize, FALSE, FALSE);
		}
		
		char const *hyp, *uttid;
		int32_t score;
		
		rv = ps_end_utt(ps);
		if(rv < 0) return "e4";
		
		hyp = ps_get_hyp(ps, &score, &uttid);
		if(hyp == NULL) return "e5";
		
		string word = hyp;
		stringstream ss(word);
		getline(ss, word, ' ');
		if(verb) cout << "hyp: " << word << ":\n";
		
		words[worditer] = word;
		worditer++;
		
		if(worditer > WORD_CACHE_SIZE-1){
			int freq = 0;
			detected = true;
			if(words[0] == "") detected = false;
			else{
				for(int i = 0; i < WORD_CACHE_SIZE; i++){
					if(words[0] == words[i]) freq++;
				}
			}
			if(freq < thresh) detected = false;
			worditer = 0;
		}

		int deltat = (int) chrono::duration<double, milli> (chrono::steady_clock::now() - start).count();
		if(verb) cout << (smallsamplesize - deltat) << "\n";
		usleep((smallsamplesize - deltat));
	}
	
	return words[0];
}

int psintf::pause(){
	arw.pause();
	return 0;
}

int psintf::resume(){
	arw.resume();
	return 0;
}

int psintf::clean(){
	ps_free(ps);
	return 0;
}
