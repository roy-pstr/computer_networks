#include <stdio.h>
#include <assert.h>
#include "scheduler.h"
#include "utils.h"

#define ARGS_NUM 4


int main(int argc, char **argv)
{

	assert(argc == ARGS_NUM);
	Args args;
	parseArgs(argv, &args); /*Doron: tested*/
	
	Files files;
	openFiles(args.name, &files); /* Doron: didnt test but should work */
	
	runScheduler();

	closeFiles(&files); /* Doron: didnt test but must work */
}


/*
Doron's functions:
parsargs
openfiles
packet input
store packet
endoffile
closefiles
cleanmem
*/

////test for parseArgs
//int main(int argc, char **argv) 
//{
//		assert(argc == ARGS_NUM);
//		Args args;
//		parseArgs(argv, &args); /* output -> int type, char * name, int size */
//		printArgs(&args);
//		return 0;
//}
