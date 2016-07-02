#include <iostream>

#include "psintf.h"
#include "ct_time.h"

using namespace std;

psintf detect;
psintf act;
psintf echo;

int init(){
	int rv = 0;
	
	rv = detect.init("libs/leplm/detect.dic");
	rv += 10*act.init("libs/leplm/act.dic");
	rv += 100*echo.init("libs/leplm/echo.dic");
	
	detect.pause();
	act.pause();
	echo.pause();
	
	return rv;
}

string socket(string data){
	cout << data << "\n";
	return "null";
}

string recognize(int mode){
	
	if(mode == 1) return detect.getword(7);
	else if(mode == 2) return act.getword(6);
	else if(mode == 3) return echo.getword(6);
	cout << "error\n";
	
	return "Logic Error";
}

int clean(){
	
	detect.clean();
	act.clean();
	echo.clean();
	
	return 0;
}

int main(int argc, char* argv[]){

	init();
	
	detect.resume();
	
	bool running = true;
	
	int mode = 1;
	bool mute = false;
	while(running){
		
		if(socket("c: check_mute") == "mute") mute = true;
		else mute = false;
		
		if(mode == 1 && !mute) socket("a: a");
		else if(mode == 2 && !mute) socket("a: b");
		else if(mode == 3 && !mute) socket("a: c");
		
		if(mute){
			cusleep(100000);
		}
		else{
			string word = recognize(mode);
			cout << "a\n";
			if(word[0] == 'e') cout << "Speech Error: " << word << "\n";
			cout << "b\n";
			socket("a: d");
			
			if(mode == 1){
				if(word == "lep"){
					mode = 2;
					detect.pause();
					act.resume();
					socket("s: What would you like me to do for you?");
				}
			}
			else if(mode == 2){
				if(word == "alpha") socket("a: 1");
				else if(word == "bravo") socket("a: 2");
				else if(word == "charlie") socket("a: 3");
				else if(word == "delta") socket("a: 4");
				else if(word == "foxtrot"){
					mode = 3;
					act.pause();
					echo.resume();
					socket("s: continue");
				}
				else if(word == "thank_you"){
					mode = 1;
					act.pause();
					detect.resume();
					socket("s: You are welcome.");
				}
			}
			else if(mode == 3){
				mode = 2;
				echo.pause();
				act.resume();
				if(word == "play") socket("m: pause");
				else if(word == "next") socket("m: next");
				else if(word == "previous") socket("m: prev");
				else if(word == "classical"){
					socket("m: clear");
					socket("m: add /home/carter/Music/Music/Classical.pls");
				}
				else if(word == "modern"){
					socket("m: clear");
					socket("m: add /home/carter/Music/Music/Modern.pls");
				}
				else if(word == "its_the_end_of_the_world"){
					socket("m: clear");
					socket("m: add /home/carter/Music/Music/Modern/endofworld");
				}
				else if(word == "good_morning") socket("s: Good morning!");
				else if(word == "good_afternoon") socket("s: Good afternoon!");
				else if(word == "good_day") socket("s: Good day!");
				else if(word == "sing") socket("s: good  day  i am the lep computer program. . .  my programmer was carter and he taught me a song. . . if you would like i can sing it for you. . . daisy,  daisy  give  me  your  answer  do . . . i  am  half  crazy  all  for  the  love  of  you . . . it  won t  be  a  stylish  marrige,  i  cant  afford  a  carriage. . . but,   you d,   look,     sweet,     upon,      the,      seat,       of,        a,        bicycle,          made,           for,            ...,two");
				else if(word == "thank_you"){
					mode = 1;
					act.pause();
					detect.resume();
					mute = true;
					socket("s: You are welcome. I will talk to you later.");
				}
			}
		}
	}
	
	clean();
	
	return 0;

}
