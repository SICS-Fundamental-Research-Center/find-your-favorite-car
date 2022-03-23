#include "preferLearn.h"
#include "non-interactive.h"


int main(int argc, char *argv[])
{ 
	// the main method for the interactive methods.

	int interactiveMode;
	printf("Press 1 to enter interactive mode and 2 to enter non-interative mode:");
	scanf("%d", &interactiveMode);
	if(interactiveMode == 1)
		demo();
	else if(interactiveMode == 2)
		non_interacitve();


	return 0;
}