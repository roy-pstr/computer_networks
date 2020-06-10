#include <stdlib.h>
#include <assert.h>
#include "string.h"
#include "flow.h"
#include "packet.h"
#include "utils.h"

/* internal functions */
void initFlow(Flow * flow, int step_size) {
	flow->curr_pckt = NULL;
	flow->cycle_steps_done = 0;
	flow->head = NULL;
	flow->next = NULL;
	flow->weight = step_size;
	flow->max_buffer = 0;
	flow->done_time = 0;
}

int countFullPackets(Flow * flow) {
	int counter = 0;
	Packet *curr_packet = flow->head;
	while (curr_packet != NULL) {
		if (pcktFull(curr_packet)) {
			counter++;
		}
		curr_packet = curr_packet->next;
	}
	return counter;
}
int countNonEmptyPackets(Flow * flow) {
	int counter = 0;
	Packet *curr_packet = flow->head;
	while (curr_packet != NULL) {
		if (!pcktEmpty(curr_packet)) {
				counter++;
		}
		curr_packet = curr_packet->next;
	}
	return counter;
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

void flowDelays(Flow * flow, stats_st *stats) {
	double totalDelay = 0;
	int tempMax = 0;
	Packet *curr_p = flow->head;
	int curr_d;
	while (NULL != curr_p) {
		//curr_d = curr_p->done_time - curr_p->arrival_time;
		curr_d = curr_p->start_time - curr_p->arrival_time;
		totalDelay += (double)curr_d;
		if (curr_d > tempMax) {
			tempMax = curr_d;
		}
		curr_p = curr_p->next;
	}
	stats->maxDelay = tempMax;
	stats->avgDelay = totalDelay / stats->pcktsNum;
}

/* external functions */
bool isPacketDonePerCycle(Flow *flow, int type, int quant_size) {
	/* verify input and get the current packet in the flow */
	assert(flow != NULL);
	Packet *pckt = flow->curr_pckt;
	assert(pckt != NULL);
	switch (type)
	{
	case RR:
		return pcktEmpty(pckt);
		break;
	case DRR:
		return pcktDone(pckt, quant_size);
		break;
	default:
		assert(false);
		break;
	}
}

void getStats(Flow * flow, stats_st *stats, int total_time) {
	stats->pcktsNum = countPackets(flow);
	flowDelays(flow, stats);
	stats->maxBuff = flow->max_buffer;
	int pckts_waiting_time = 0;
	Packet *tmp = flow->head;
	while (tmp != NULL) {
		pckts_waiting_time += tmp->start_time - tmp->arrival_time;
		tmp = tmp->next;
	}

	stats->avgBuff = (double)pckts_waiting_time / total_time;
}

Flow *nextNonEmptyFlow(Flow *head, Flow *flow) {
	assert(NULL != head);
	assert(NULL != flow);
	Flow *init_f = flow;
	flow = (flow->next==NULL) ? head : flow->next;
	/* cyclic search for the next flow in list that is NOT empty */
	/* if reached same flow as starting flow, and it is not empty, return the starting flow */
	/* if all flows in list are empty, return NULL */
	while (flowEmpty(flow)) {
		flow = (flow->next == NULL) ? head : flow->next;
		if (init_f == flow) {
			break;
		}
	}
	if (flowEmpty(flow)) {
		return NULL;
	}
	return flow;
}

bool flowEmpty(Flow *flow) {
	assert(NULL != flow);
	/* if the current packet of the flow is NULL - the flow is empty, no more packet to send */
	return (NULL == flow->curr_pckt);
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
	packet->arrival_time = atoi(curr_line_ptr);

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
		if (flowsAreEqual(flow_head, new_flow))
			return flow_head;
		flow_head = flow_head->next;
		if (flow_head == NULL)
			return NULL;

		
	}
}

void storePacket(const char *line, Flow **flow_head, int step_size)
{
	Flow given_flow;
	initFlow(&given_flow, step_size);

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

	/* update flow stats */
	if (curr_flow->max_buffer < countFullPackets(curr_flow)) {
		curr_flow->max_buffer = countFullPackets(curr_flow);
	}
}





