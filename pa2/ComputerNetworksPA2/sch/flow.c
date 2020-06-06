#include <stdlib.h>
#include <assert.h>
#include "string.h"
#include "flow.h"


void flowNextPacket(flow_st * flow)
{
	flow->curr_pckt = flow->curr_pckt->next;
}

packet_st * getPacket(flow_st * flow)
{
	return flow->curr_pckt;
}

void flowStep(flow_st *flow, int step_size) {
	assert(flow != NULL);
	packet_st *pckt = getPacket(flow);
	assert(pckt != NULL);
	pcktStep(pckt, step_size);
	flow->curr_num_of_steps_done++;
	if (pcktDone(pckt)) {
		flowNextPacket(flow);
	}
}

flow_st *nextFlow(flow_st *flow) {
	flow->curr_num_of_steps_done = 0;
	return flow->next;
}

bool floesAreEqual(flow_st *f1, flow_st *f2)
{
	if (f1->dadd != f2->dadd)
		return false;
	if (f1->sadd != f2->dadd)
		return false;
	if (strcmp(f1->dadd, f2->dadd))
		return false;
	if (strcmp(f1->sadd, f2->sadd))
		return false;

	return true;
}


