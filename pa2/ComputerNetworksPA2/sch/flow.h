#ifndef _FLOW_H
#define _FLOW_H
#include <stdbool.h>
#include "packet.h"

enum PACKET_STATUS {
	SAME_PACKET = 0,
	NEXT_PACKET = 1,
	FAILED = 2
};

typedef struct flow_st
{
	int id;
	int weight;
	int num_of_steps_done;
	packet_st** head;
	packet_st* curr_pckt;
	flow_st *next;
}flow_st;

enum PACKET_STATUS flowStep(flow_st *flow, int step_size);

flow_st *nextPacket(flow_st *flow);
#endif


