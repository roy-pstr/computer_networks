#include <string.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include "utils.h"


void parseArgs(char **argv, Args *args)
{
	if (!strcmp(argv[1], "RR"))
		args->scheduler_type = RR;
	else if (!strcmp(argv[1], "DRR"))
		args->scheduler_type = DRR;
	else
		assert(false);

	args->name = argv[2];

	args->size = atoi(argv[3]);	 
}

/* debugging */
void printArgs(Args *args)
{
	printf("------Printing Args ------\n");
	printf("scheduler_type = ");
	if (args->scheduler_type == RR)
		printf("RR\n");
	else if (args->scheduler_type == DRR)
		printf("DRR\n");
	else
		assert(false);
	printf("name = %s\n", args->name);
	printf("size = %d\n", args->size);

	printf("----Done Printing Args ----\n");
}

void openOneFile(const char *head_path, const char *tail_path, FILE **fp, const char *mode)
{
	int filename_len = strlen(head_path) + strlen(tail_path) + 1;
	char *file_path = (char *)malloc(filename_len * sizeof(char));
	assert(file_path);

	strcpy(file_path, head_path);
	strcat(file_path, tail_path);
	*fp = fopen(file_path, mode);
	assert(*fp);

	free(file_path);
}


void openFiles(const char *head_path, Files *files)
{
	openOneFile(head_path, "_in.txt",    &(files->input_file),  "r");
	openOneFile(head_path, "_out.txt",   &(files->output_file), "w");
	openOneFile(head_path, "_stat.txt",  &(files->input_file),  "w");
}

