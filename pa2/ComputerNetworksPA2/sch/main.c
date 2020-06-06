#include <stdio.h>
#include <assert.h>
#include "scheduler.h"
#include "utils.h"

#define ARGS_NUM 4


//int main(int argc, char **argv)
//{
//
//	assert(argc == ARGS_NUM);
//	Args args;
//	parseArgs(argv, &args); /* output -> int type, char * name, int size */
//	
//	openFiles(); /* return pointer to the input file, output file and stats file */
//	
//	runScheduler();
//
//	closeFiles();
//}


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
