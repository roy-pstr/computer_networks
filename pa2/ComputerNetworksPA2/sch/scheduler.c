#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "utils.h"
#include "scheduler.h"
#include "packet.h"
#include "flow.h"


Flow *schedulerStep(Flow *head, Flow *curr_flow, int type, int step_size, int time, FILE * log_file) {
	if (flowStepDone(curr_flow, type)) {
		curr_flow->cycle_steps_done++;
	}
	
	/* preforms one step = sends the basic quentom size of bytes per time unit from the current flow */
	//flowStep(curr_flow, step_size, time, log_file);

	/* procceds to next flow or stay in current flow */
	if (curr_flow->cycle_steps_done == curr_flow->weight) {
		curr_flow->cycle_steps_done = 0;
		curr_flow = findNextNonEmptyFlow(head, curr_flow);
	}
	if (flowEmpty(curr_flow)) {
		//flow->cycle_steps_done = 0; TBA -> check if we need to zero the steps counter even when flow empty but not done sending all packets per cycle!
		curr_flow = findNextNonEmptyFlow(head, curr_flow);
	}
	return curr_flow;
}

void runScheduler(Args *args, Files *files)
{
	Flow *flow_head = NULL;
	Flow *curr_flow = NULL;
	int time = 0;
	bool flows_stack_empty = false;
	bool packets_input_done = false;
	
	char line[MAX_LINE_LEN];
	/* read first line */
	if (fgets(line, MAX_LINE_LEN, files->input_file) == NULL) {
		/* empty input file ? */
		assert(false);
	}

	/* loop over time: every iter is one time unit step */
	while (!flows_stack_empty) {
		
		if (packets_input_done == false) {
			/* check if it is time to recive the packet */
			while (timeToRecivePacket(line, time)) {
				/* store packet in the flow stack. */
				storePacket(line, &flow_head);
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
		

		/*	preform one scheduler step.
			sends the next quantom bytes unit 
			return the same flow	if more bytes to be sent from this flow 
									otherwise will return the next flow in the queue
			if returns NULL -> all flows are empty */
		if (curr_flow != NULL) {
			sendByte(curr_flow);
			curr_flow = schedulerStep(flow_head, curr_flow,args->scheduler_type, args->step_size, time, files->output_file);
		}

		/* exit loop condition */
		if (curr_flow == NULL && packets_input_done) {
			/* finished send all the packets */
			flows_stack_empty = true;
			/*Doron: break? */
		}
		
		/* time unit step */
		time++; 
	}

	writeStats(flow_head, files->stats_file);
	//cleanMem();
}