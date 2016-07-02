#include <iostream>

#include "psintf.h"

using namespace std;

int main(int argc, char* argv[]){
	
	psintf ps;
	
	ps.init("libs/leplm/detect.dic", false, true);
	while(1==1) cout << ps.getword() << "\n";
	ps.clean();
	
	return 0;
}
