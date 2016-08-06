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

#include <pocketsphinx.h>
#include <sphinxbase/cont_ad.h>

class psintf {

public:
	int init(std::string dict, std::string lm, std::string hmm, bool psv = false, bool v = false);
	
	std::string getword(int thresh=7);
	
	int clean();
	
	int pause();
	int resume();
	
private:
	ad_rec_t * ad;
	cont_ad_t * cont;
	int cycles_since_calib;
	ps_decoder_t *ps;
	bool verb;

	// How long to wait during silence until utterance is finished
	const int SILENCE_DELAY = 4000; // In samples
};
