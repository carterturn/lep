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

#include "lepmodule.h"

using namespace std;

string lepmodule::getparam(string paramname, vector<configtuple> params, string defaultval){
    for(int i = 0; i < params.size(); i++){
        if(params[i].param == paramname){
            return params[i].value;
        }
    }
    
    return defaultval;
}

lepmodule::lepmodule(string pw, string k){
    password = pw;
    key = k;
}
