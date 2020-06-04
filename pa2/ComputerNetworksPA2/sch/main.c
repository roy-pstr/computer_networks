#include "scheduler.h"
#include <stdio.h>

int main(int argc, char **argv){
	
	parseArgs(); /* output -> int type, char * name, int size */
	
	openFiles(); /* return pointer to the input file, output file and stats file */
	
	runScheduler();

	closeFiles();

}

/*Doron's functions:
parsargs
openfiles
packet input
store packet
endoffile
closefiles
cleanmem
/*

