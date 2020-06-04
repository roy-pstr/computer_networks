#include <stdlib.h>
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
	if (flow == NULL){
		return FAILED;
	}
	packet_st *pckt = getPacket(flow);
	if (pckt == NULL){
		return FAILED;
	}
	pcktStep(pckt, step_size);
	flow->curr_num_of_steps_done++;
	if (pcktDone(pckt)) {
		flowNextPacket(flow);
	}
}

flow_st *nextFlow(flow_st *flow) {
	flow->num_of_steps_done = 0;
	return flow->next;
}

