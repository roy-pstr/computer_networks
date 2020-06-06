#include <stdlib.h>
#include <assert.h>
#include "string.h"
#include "flow.h"
#include "packet.h"
#include "utils.h"

#define NO_WEIGHT_GIVEN -1

/* internal functions */
void nextPacket(flow_st * flow)
{
	flow->curr_pckt = flow->curr_pckt->next;
}

packet_st * getPacket(flow_st * flow)
{
	return flow->curr_pckt;
}

int countPackets(flow_st * flow) {
	int counter = 0;
	packet_st *curr_packet = flow->head;
	while (curr_packet != NULL) {
		counter++;
		curr_packet = curr_packet->next;
	}
	return counter;
}

void getStats(flow_st * flow, stats_st *stats) {
	stats->pcktsNum = countPackets(flow);
}

flow_st *addNewFlow(flow_st **flow_head, flow_st *new_flow) {

}

void addNewPacket(flow_st *flow, packet_st *pckt) {
}

/* external functions */
void writeStats(flow_st * head, FILE * stats_file) {
	flow_st *curr_f = head;
	stats_st curr_stats;
	while (curr_f != NULL) {
		getStats(curr_f, &curr_stats);
		writeStatLog(stats_file, curr_f->id, curr_stats.pcktsNum, curr_stats.maxDelay, curr_stats.avgDelay, curr_stats.maxBuff, curr_stats.avgBuff);
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
		nextPacket(flow);
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
	return !(strcmp(f1->id, f2->id));
}


void parse_line(const char *line, flow_st *flow, packet_st *packet)
{
	char *curr_line_ptr = line;			//points on pktID
	packet->id = atoi(curr_line_ptr);

	curr_line_ptr = getPointerAfterSpace(curr_line_ptr); //points on Time
	packet->start_time = atoi(curr_line_ptr);

	curr_line_ptr = getPointerAfterSpace(curr_line_ptr); //points on Sadd
	char *end_of_flow_id_ptr = curr_line_ptr;

	for (int i = 0; i < 4; i++) //skip 4 spaces
		end_of_flow_id_ptr = getPointerAfterSpace(end_of_flow_id_ptr);

	end_of_flow_id_ptr--; //points on end of Dport

	for (int i = 0; curr_line_ptr < end_of_flow_id_ptr; i++, curr_line_ptr++)
		flow->id[i] = *curr_line_ptr;

	curr_line_ptr = getPointerAfterSpace(curr_line_ptr); //points on length
	packet->curr_len = atoi(curr_line_ptr);

	curr_line_ptr = getPointerAfterSpace(curr_line_ptr); //points on weight or NULL
	if (curr_line_ptr == NULL)
		flow->weight = NO_WEIGHT_GIVEN;
	else
		flow->weight = atoi(curr_line_ptr);
}

flow_st *getFlowPointer(flow_st *flow_head, flow_st *new_flow)
{
	if (flow_head == NULL)
		return NULL;

	while (true)
	{
		flow_head = flow_head->next;
		if (flow_head == NULL)
			return NULL;

		if (flowsAreEqual(flow_head, new_flow))
			return flow_head;
	}
}

void storePacket(const char *line, flow_st **flow_head)
{
	flow_st new_flow;
	packet_st new_packet;

	parse_line(line, &new_flow, &new_packet);

	flow_st *new_flow_ptr = getFlowPointer(*flow_head, &new_flow); 
	if (new_flow_ptr == NULL)
		new_flow_ptr = addNewFlow(flow_head, &new_flow); //remmember to allocate memory. attention - function returns flow pointer
	
	addNewPacket(new_flow_ptr, &new_packet); //remmember to allocate memory
}



