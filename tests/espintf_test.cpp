#include <iostream>

#include "espintf.h"

using namespace std;

int main(int argc, char* argv[]){
	
	espintf esp;
	
	esp.speak("Testing 1 2 3");
	
	esp.speak("You look pretty today!");
	
	return 0;
}
