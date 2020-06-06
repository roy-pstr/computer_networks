#include <stdlib.h>
#include <assert.h>
#include "string.h"
#include "flow.h"
#include "packet.h"
#include "utils.h"

/* internal functions */
void initFlow(Flow * flow) {
	flow->curr_pckt = NULL;
	flow->cycle_steps_done = 0;
	flow->head = NULL;
	flow->next = NULL;
	flow->weight = 1;
}
void nextPacket(Flow * flow)
{
	flow->curr_pckt = flow->curr_pckt->next;
}

Packet * getPacket(Flow * flow)
{
	return flow->curr_pckt;
}

int countPackets(Flow * flow) {
	int counter = 0;
	Packet *curr_packet = flow->head;
	while (curr_packet != NULL) {
		counter++;
		curr_packet = curr_packet->next;
	}
	return counter;
}

void getStats(Flow * flow, stats_st *stats) {
	stats->pcktsNum = countPackets(flow);
}

Flow *addNewFlow(Flow **head, Flow *new_flow) {
	Flow *last_flow = *head;
	
	/* first flow in list */
	if (last_flow == NULL) {
		*head = new_flow;
		return *head;
	}

	/* otherwise, find last flow in list */
	while (last_flow->next != NULL) {
		last_flow = last_flow->next;
	}
	last_flow->next = new_flow;
	return last_flow->next;
}

void addNewPacket(Flow *flow, Packet *pckt) {
	Packet *last_pckt = flow->head;
	/* no packets in flow yet */
	if (last_pckt == NULL) {
		flow->head = pckt;
	}
	else{
		/*otherwise, add packet to the end of packet list */
		while (last_pckt->next != NULL) {
			last_pckt = last_pckt->next;
		}
		last_pckt->next = pckt;
	}
	/* case when adding new packet the the curr_packet pointer is pointing to NULL, can happen due to:
		1. first initailazation 
		2. flow sent all packets so pointing to NULL and new packet arrives. */
	if (flow->curr_pckt == NULL) {
		flow->curr_pckt = pckt;
	}
}

/* external functions */
bool flowStepDone(Flow *flow, int type, int quant_size) {
	/* verify input and get the current packet in the flow */
	assert(flow != NULL);
	Packet *pckt = getPacket(flow);
	assert(pckt != NULL);
	switch (type)
	{
	case RR:
		return pcktDone(pckt, 0);
		break;
	case DRR:
		return pcktDone(pckt, quant_size);
		break;
	default:
		assert(false);
		break;
	}
}
void sendByte(Flow *flow) {
	/* verify input and get the current packet in the flow */
	assert(flow != NULL);
	Packet *pckt = getPacket(flow);
	assert(pckt != NULL);
	pcktStep(pckt, 1);
}

void writeStats(Flow * head, FILE * stats_file) {
	Flow *curr_f = head;
	stats_st curr_stats;
	while (curr_f != NULL) {
		getStats(curr_f, &curr_stats);
		writeStatLog(stats_file, curr_f->id, curr_stats.pcktsNum, curr_stats.maxDelay, curr_stats.avgDelay, curr_stats.maxBuff, curr_stats.avgBuff);
		curr_f = curr_f->next;
	}
}

void flowStep(Flow *flow, int step_size, int time, FILE * log_file) {
	/* verify input and get the current packet in the flow */
	assert(flow != NULL);
	Packet *pckt = getPacket(flow);
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

Flow *findNextNonEmptyFlow(Flow *head, Flow *flow) {
	assert(NULL != head);
	assert(NULL != flow);
	Flow *start_f = flow;
	Flow *curr_f = flow;

	do {
		curr_f = curr_f->next;
		/* check if we went over ALL flows in list */
		if (curr_f == start_f) {
			/* all flows in list are empty! */
			return NULL;
		}
		/* if we reached the end of the list, go back to the begging */
		if (curr_f == NULL) {
			curr_f = head;
		}
	} while (flowEmpty(curr_f));
	return curr_f;
}

bool flowEmpty(Flow *flow) {
	assert(NULL != flow);
	/* if the current packet of the flow is NULL - the flow is empty, no more packet to send */
	return (NULL == getPacket(flow));
}

bool flowsAreEqual(Flow *f1, Flow *f2)
{
	return !(strcmp(f1->id, f2->id));
}

void parseLine(const char *line, Flow *flow, Packet *packet)
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

	int i;
	for (i = 0; curr_line_ptr < end_of_flow_id_ptr; i++, curr_line_ptr++)
		flow->id[i] = *curr_line_ptr;
	flow->id[i] = '\0';

	curr_line_ptr = getPointerAfterSpace(curr_line_ptr); //points on length
	packet->len = packet->curr_len = atoi(curr_line_ptr);

	curr_line_ptr = getPointerAfterSpace(curr_line_ptr); //points on weight or NULL
	if (curr_line_ptr != NULL)
		flow->weight = atoi(curr_line_ptr);
}

Flow *findFlow(Flow *flow_head, Flow *new_flow)
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

void storePacket(const char *line, Flow **flow_head)
{
	Flow given_flow;
	initFlow(&given_flow);

	Packet *new_packet = (Packet *)malloc(sizeof(Packet));
	assert(NULL != new_packet);
	initPacket(new_packet);

	parseLine(line, &given_flow, new_packet);

	/* search the given flow in the flow list */
	Flow *curr_flow = findFlow(*flow_head, &given_flow); // attention - function returns flow pointer
	
	/* if flow not exist, create new flow and add to end of list */
	if (curr_flow == NULL) {
		Flow *new_flow = (Flow *)malloc(sizeof(Flow));
		*new_flow = given_flow; /* deep copy flow */
		curr_flow = addNewFlow(flow_head, new_flow); // attention - function returns flow pointer
	}

	/* add packet to flow */
	addNewPacket(curr_flow, new_packet); //remmember to allocate memory
}



