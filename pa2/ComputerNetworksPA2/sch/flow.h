#ifndef _FLOW_H
#define _FLOW_H
#include <stdbool.h>
#include "packet.h"
#include "utils.h"
#include <stdio.h>

typedef struct stats_st
{
	int pcktsNum;
	int maxDelay;
	double avgDelay; 
	int maxBuff;
	double avgBuff;

}stats_st;

typedef struct flow_st
{
	char id[MAX_LINE_LEN]; /* "Sadd Sport Dadd Dport" */

	int weight;
	int cycle_steps_done;

	// queue of packets:
	Packet* head;
	Packet* curr_pckt;

	//flow linked list
	struct flow_st *next;
}Flow;

bool flowStepDone(Flow * flow, int type);

void writeStats(Flow * head, FILE * stats_file);

bool flowEmpty(Flow * flow);

bool flowsAreEqual(Flow *f1, Flow *f2);

void flowStep(Flow *flow, int step_size, int time, FILE * log_file);

Flow *findNextNonEmptyFlow(Flow *head, Flow *flow);

void storePacket(const char *line, Flow **flow_head);

//addPckt(flow_st *f) {
//	/* add packet at the end of the list */
//}

#endif


