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

	// statistics:
	int max_buffer;
	// int arrival_time; - no need, same as head->arrival_time;
	int done_time;

	// queue of packets:
	Packet* head;
	Packet* curr_pckt;

	//flow linked list
	struct flow_st *next;
}Flow;

bool flowEmpty(Flow * flow);

bool flowsAreEqual(Flow *f1, Flow *f2);

int countFullPackets(Flow * flow);
int countNonEmptyPackets(Flow * flow);
int countPackets(Flow * flow);

bool isPacketDonePerCycle(Flow * flow, int type, int quant_size);

void getStats(Flow * flow, stats_st * stats, int total_time);

Flow *nextNonEmptyFlow(Flow *head, Flow *flow);

void storePacket(const char *line, Flow **flow_head, int step_size);

//addPckt(flow_st *f) {
//	/* add packet at the end of the list */
//}

#endif


