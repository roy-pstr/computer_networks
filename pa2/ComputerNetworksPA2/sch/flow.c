#include <stdlib.h>
#include <assert.h>
#include "string.h"
#include "flow.h"

/* internal functions */
void flowNextPacket(flow_st * flow)
{
	flow->curr_pckt = flow->curr_pckt->next;
}

packet_st * getPacket(flow_st * flow)
{
	return flow->curr_pckt;
}

void getStats(flow_st * flow, stats_st *stats) {

}

/* external functions */
void writeStats(flow_st * head, FILE * stats_file) {
	flow_st *curr_f = head;
	stats_st curr_stats;
	while (curr_f != NULL) {
		getStats(curr_f, &curr_stats);
		writeStatLog(stats_file, curr_f->id, curr_f->pckts_num, curr_stats.maxDelay, curr_stats.avgDelay, curr_stats.maxBuff, curr_stats.avgBuff);
		curr_f = curr_f->next;
	}
}

void flowStep(flow_st *flow, int step_size, int time, FILE * log_file) {
	/* verify input and get the current packet in the flow */
	assert(flow != NULL);
	packet_st *pckt = getPacket(flow);
	assert(pckt != NULL);

	/* check if this is the first time proccess the current packet */
	if (pcktStart(pckt)) {
		/* write the time: id to log*/
		writeLog(log_file, time, pckt->id);
	}

	/* preform one step = sends the basic quentom size of bytes per time unit from the current packet */
	pcktStep(pckt, step_size);

	/* update the number of steps already done in this flow per cycle */
	flow->cycle_steps_done++;

	/* check if the current packet was fully sent after the last step */
	if (pcktDone(pckt)) {
		/* procced to the next packet inside this flow */
		flowNextPacket(flow);
	}
}

flow_st *findNextNonEmptyFlow(flow_st *head, flow_st *flow) {
	assert(NULL != head);
	assert(NULL != flow);
	flow_st *start_f = flow;
	flow_st *curr_f = flow;

	while (flowEmpty(curr_f)) {
		if (curr_f == start_f) {
			/* all flows in list are empty! */
			return NULL;
		}
		curr_f = curr_f->next;
		if (curr_f == NULL) {
			/* if reached the last flow, go back to head */
			curr_f = head;
		}
	}
	return curr_f;
}

bool flowEmpty(flow_st *flow) {
	assert(NULL == flow);
	/* if the current packet of the flow is NULL - the flow is empty, no more packet to send */
	return (NULL == getPacket(flow));
}

bool flowsAreEqual(flow_st *f1, flow_st *f2)
{
	if (strcmp(f1->id, f2->id))
		return false;

	return true;
}


