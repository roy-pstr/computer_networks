#include <string.h>
#include <assert.h>
#include <stdbool.h>

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

	args->step_size = atoi(argv[3]);	 
}

/* debugging function */
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
	printf("step_size = %d\n", args->step_size);

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

void closeFiles(Files *files)
{
	fclose(files->input_file);
	fclose(files->output_file);
	fclose(files->stats_file);
}

void writeLog(FILE *fp, int time, int id) {
	/* writes time: id to fp*/
	char log_line[MAX_LINE_LEN];
	char time_str[MAX_LINE_LEN];
	char id_str[MAX_LINE_LEN];
	sprintf(time_str, "%d", time);
	sprintf(id_str, "%d", id);
	strcpy(log_line, time_str);
	strcat(log_line, ": ");
	strcat(log_line, id_str);
	fprintf(fp, "%s\n", log_line);
}

void writeStatLog(FILE *fp, char *flow, int numPkts, int maxDelay, double avgDelay, int maxBuff, double avgBuff) {
	/* writes 'Sadd Sport Dadd Dport numPkts maxDelay avgDelay maxBuff avgBuff' to fp*/
	char log_line[MAX_LINE_LEN];
	char numPkts_str[MAX_LINE_LEN];
	char maxDelay_str[MAX_LINE_LEN];
	char avgDelay_str[MAX_LINE_LEN]; 
	char maxBuff_str[MAX_LINE_LEN];
	char avgBuff_str[MAX_LINE_LEN];
	sprintf(numPkts_str, "%d", numPkts);
	sprintf(maxDelay_str, "%d", maxDelay);
	sprintf(avgDelay_str, "%.2f", avgDelay);
	sprintf(maxBuff_str, "%d", maxBuff);
	sprintf(avgBuff_str, "%.2f", avgBuff);
	strcpy(log_line, flow);
	strcat(log_line, " ");
	strcat(log_line, numPkts_str);
	strcat(log_line, " ");
	strcat(log_line, maxDelay_str);
	strcat(log_line, " ");
	strcat(log_line, avgDelay_str);
	strcat(log_line, " ");
	strcat(log_line, maxBuff_str);
	strcat(log_line, " ");
	strcat(log_line, avgBuff_str);
	fprintf(fp, "%s\n", log_line);
}