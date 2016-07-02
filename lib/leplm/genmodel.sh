#!/bin/bash

../../../recognizer/CMU-Cam_Toolkit_v2/bin/text2wfreq < $1 | ../../../recognizer/CMU-Cam_Toolkit_v2/bin/wfreq2vocab > $2.vocab
../../../recognizer/CMU-Cam_Toolkit_v2/bin/text2idngram -vocab $2.vocab < $1 > $2.idngram
../../../recognizer/CMU-Cam_Toolkit_v2/bin/idngram2lm -vocab_type 0 -idngram $2.idngram -vocab $2.vocab -arpa $2.arpa
sphinx_lm_convert -i $2.arpa -o $2.lm.DMP
