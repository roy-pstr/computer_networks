#ifndef _FLOW_H
#define _FLOW_H
#include <stdbool.h>
#include "packet.h"
#include "utils.h"

typedef struct flow_st
{
	//int id;
	char sadd[ID_STR_LEN];
	int sport;
	char dadd[ID_STR_LEN];
	int dport;

	int weight;
	int curr_num_of_steps_done;

	// queue of packets:
	packet_st* head;
	packet_st* curr_pckt;

	//flow linked list
	struct flow_st *next;
}flow_st;

bool floesAreEqual(flow_st *f1, flow_st *f2);

enum PACKET_STATUS flowStep(flow_st *flow, int step_size);

addPckt(flow_st *f) {
	/* add packet at the end of the list */
}

#endif


