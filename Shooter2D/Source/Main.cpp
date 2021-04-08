#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")

#include <string>
#include "IO.h"

bool STRESS_TEST;


int main(int argc, char** argv)
{
	

	if (argc > 1) {
		if (std::string(argv[1]) == "-stress") {
			STRESS_TEST = true;
		}
	}
	IO io("Game");
	io.start();
	
	return 0;
}