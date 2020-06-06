#ifndef _FLOW_H
#define _FLOW_H
#include <stdbool.h>
#include "packet.h"
#include "utils.h"
#include <stdio.h>

typedef struct stats_st
{
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
	packet_st* head;
	packet_st* curr_pckt;

	//flow linked list
	struct flow_st *next;
}flow_st;

void writeStats(flow_st * head, FILE * stats_file); 

bool flowEmpty(flow_st * flow);

bool flowsAreEqual(flow_st *f1, flow_st *f2);

void flowStep(flow_st *flow, int step_size, int time, FILE * log_file);

flow_st *findNextNonEmptyFlow(flow_st *head, flow_st *flow);

void storePacket(const char *line, flow_st **flow_head);

//addPckt(flow_st *f) {
//	/* add packet at the end of the list */
//}

#endif


