#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <assert.h>
#include "utils.h"
#include "scheduler.h"
#include "packet.h"
#include "flow.h"

void cleanMem(Flow *head) {
	Flow *curr_f = head;
	Flow *temp;
	while (NULL != curr_f) {
		temp = curr_f;
		curr_f = curr_f->next;
		freePacketList(temp->head);
		free(temp);
	} 
}

void writeStats(Flow * head, FILE * stats_file, int total_time) {
	Flow *curr_f = head;
	stats_st curr_stats;
	while (curr_f != NULL) {
		getStats(curr_f, &curr_stats, total_time);
		writeStatLog(stats_file, curr_f->id, curr_stats.pcktsNum, curr_stats.maxDelay, curr_stats.avgDelay, curr_stats.maxBuff, curr_stats.avgBuff);
		curr_f = curr_f->next;
	}
}

void sendByte(Flow *flow) {
	/* verify input and get the current packet in the flow */
	assert(flow != NULL);
	Packet *pckt = flow->curr_pckt;
	assert(pckt != NULL);
	pcktStep(pckt, 1);
}

Flow * nextPacket(Flow *head, Flow *curr_flow, int time) {
	/* move to next packet inside the current flow */
	curr_flow->curr_pckt = curr_flow->curr_pckt->next;
	/* in case the flow is empty (even if not done all cycle steps inside flow) -> move to next flow */
	if (NULL == curr_flow->curr_pckt) {
		curr_flow->done_time = time; /* in case it is the last packet */
		curr_flow->cycle_steps_done = 0; //TBA -> check if we need to zero the steps counter even when flow empty but not done sending all packets per cycle!
		curr_flow = nextNonEmptyFlow(head, curr_flow);
	}

	/* check if finish sending packets per cycle from the current flow */
	else if ((curr_flow->cycle_steps_done == curr_flow->weight) ){
		curr_flow->cycle_steps_done = 0;
		/* move to next non empty flow */
		curr_flow = nextNonEmptyFlow(head, curr_flow);
	}
	
	
	return curr_flow;
}

void runScheduler(Args *args, Files *files)
{
	Flow *flow_head = NULL;
	Flow *curr_flow = NULL;
	int time = 0;
	bool packets_input_done = false;
	
	char line[MAX_LINE_LEN];
	/* read first line */
	if (fgets(line, MAX_LINE_LEN, files->input_file) == NULL) {
		/* empty input file ? */
		assert(false);
	}

	/* loop over time: every iter is one time unit step */
	while (true) {
		/* handles input packets */
		if (packets_input_done == false) {
			/* check if it is time to recive the packet */
			while (timeToRecivePacket(line, time)) {
				/* store packet in the flow stack. */
				storePacket(line, &flow_head, args->step_size);
				if (curr_flow == NULL) {
					/* first flow to be added, link to curr_flow */
					curr_flow = flow_head;
				}
				/* read next line (packet) */
				if (fgets(line, MAX_LINE_LEN, files->input_file) == NULL) {
					packets_input_done = true; /* no more packets in the input file */
					break;
				}
			}
		}
		
		/* handles the bytes "sending" per time unit */
		if (curr_flow != NULL) {
			/* check if this is the first byte being sent from this packet */
			if (pcktStart(curr_flow->curr_pckt)) {
				/* write log: "time: id/n" */
				curr_flow->curr_pckt->start_time = time;
				writeLog(files->output_file, curr_flow->curr_pckt);
			}

			/* sends one byte from the current packet in the current flow */
			assert(!pcktEmpty(curr_flow->curr_pckt));
			sendByte(curr_flow); 

			/* check if done sending all the bytes from the current packet per cycle */
			if (isPacketDonePerCycle(curr_flow, args->scheduler_type, args->step_size)) {
				/*check if finish sending the whole packet */
				if (pcktEmpty(curr_flow->curr_pckt)) {
					curr_flow->curr_pckt->done_time = time;
					curr_flow->done_time = time;
				}
				curr_flow->cycle_steps_done++;
				/* move to next packet. could be in same flow or the next non empty flow */
				if (curr_flow->curr_pckt->id == 63) {
					int a = 1;
				}
				curr_flow=nextPacket(flow_head, curr_flow,time);
			}
		}

		/* time unit step */
		time++;

		/* exit condition */
		if (curr_flow == NULL && packets_input_done) {
			/* finished sending all the packets */
			break;
		}
	}
	
	writeStats(flow_head, files->stats_file,time);
	cleanMem(flow_head);
}