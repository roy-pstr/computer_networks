#ifndef _FLOW_H
#define _FLOW_H
#include <stdbool.h>
#include "packet.h"
#include "utils.h"
#include <stdio.h>

typedef struct flow_st
{
	char id[MAX_LINE_LEN]; /* "Sadd Sport Dadd Dport" */

	int weight;
	int cycle_steps_done;

	// queue of packets:
	packet_st* head;
	packet_st* curr_pckt;
	int pckts_num;

	//flow linked list
	struct flow_st *next;
}flow_st;

bool flowEmpty(flow_st * flow);

bool floesAreEqual(flow_st *f1, flow_st *f2);

void flowStep(flow_st *flow, int step_size, int time, FILE * log_file);

flow_st * nextFlow(flow_st * flow);

addPckt(flow_st *f) {
	/* add packet at the end of the list */
}

#endif


