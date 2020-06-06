#ifndef _UTILS_H
#define _UTILS_H
#include "scheduler.h"
#include <stdio.h>

#define FLOW_MAX_NUM 32000
#define ID_STR_LEN 13
#define MAX_LINE_LEN 1000 //Doron: this one should be verified

typedef struct Args
{
	int scheduler_type;
	char *name;
	int step_size;
} Args;

typedef struct Files
{
	FILE *input_file;
	FILE *output_file;
	FILE *stats_file;
}Files;

void parseArgs(char **argv, Args *args);
void printArgs(Args *args); //debugging function
void openFiles(const char *path, Files *files);
void closeFiles(Files *files);

void writeLog(FILE * fp, int time, int id);

void writeStatLog(FILE * fp, char * flow, int numPkts, int maxDelay, double avgDelay, int maxBuff, double avgBuff);

#endif

