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
	
	runScheduler(&args, &files);

	closeFiles(&files); /* Doron: didnt test but must work */
}


/*
Doron's functions:
parsargs - done & tested
openfiles - done
packet input - done, changed to timeToSendPacket
store packet - 
endoffile - fgets handles this.
closefiles - done
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


/* Dorons comments:
-time should be long int(?)
*/