#include <stdlib.h>
#include "flow.h"

void flowNextPacket(flow_st * flow)
{
	flow->curr_pckt = flow->curr_pckt->next;
}

bool flowStepsDone(flow_st *flow) {
	return (flow->num_of_steps_done == flow->weight);
}

packet_st * getPacket(flow_st * flow)
{
	return flow->curr_pckt;
}

enum PACKET_STATUS flowStep(flow_st *flow, int step_size) {
	if (flow == NULL){
		return FAILED;
	}
	packet_st *pckt = getPacket(flow);
	if (pckt == NULL){
		return FAILED;
	}
	pcktStep(pckt, step_size);
	flow->num_of_steps_done++;
	if (pcktDone(pckt)) {
		return NEXT_PACKET;
	}
	return SAME_PACKET;
}

flow_st *nextFlow(flow_st *flow) {
	flow->num_of_steps_done = 0;
	return flow->next;
}

flow_st *nextPacket(flow_st *flow) {
	if (flowStepsDone(flow)) {
		return nextFlow(flow);
	}
	else {
		flowNextPacket(flow); /* TODO: handle case of last packet! */
		return flow;
	}
}