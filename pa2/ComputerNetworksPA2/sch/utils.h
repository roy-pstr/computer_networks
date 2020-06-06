#ifndef _UTILS_H
#define _UTILS_H
#include "scheduler.h"

#define FLOW_MAX_NUM 32000

typedef struct Args
{
	int scheduler_type;
	char *name;
	int size;
} Args;

typedef struct Files
{
	FILE *input_file;
	FILE *output_file;
	FILE *stats_file;
}Files;

void parseArgs(char **argv, Args *args);
void printArgs(Args *args); //debugging
void openFiles(const char *path, Files *files);


#endif

