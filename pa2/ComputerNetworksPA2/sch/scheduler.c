#include <stdio.h>
#include <stdbool.h>
#include "utils.h"
#include "scheduler.h"
#include "packet.h"
#include "flow.h"


flow_st *schedulerStep(flow_st *head, flow_st *curr_flow, int step_size, int time, FILE * log_file) {
	/* preforms one step = sends the basic quentom size of bytes per time unit from the current flow */
	flowStep(curr_flow, step_size, time, log_file);

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
	flow_st *flow_head = NULL;
	flow_st *curr_flow = NULL;
	int time = 0;
	bool flows_stack_empty = false;
	bool no_more_inputs = false;
	
	/* loop over time: every iter is one time unit step */
	while (!flows_stack_empty) {
		char line[MAX_LINE_LEN];

		if (fgest(line, MAX_LINE_LEN, files->input_file) == NULL)
			no_more_inputs = true;

		/* every iter: check if any packets arrives in this time unit */
		while (timeToSendPacket(line, time)) {
			/* if any packet arrived, store them in the flow stack. */
			/* can handle more than 1 packet per time unit! */

			storePacket(line, &flow_head);
			if (fgest(line, MAX_LINE_LEN, files->input_file) == NULL) {
				no_more_inputs = true;
				break;
			}
		}

		/* check if there are more packets in the input file */
		//if (endOfFile(files->input_file)) {
		//	no_more_inputs = true;
		//}

		/*	preform one scheduler step.
			sends the next quantom bytes unit 
			return the same flow	if more bytes to be sent from this flow 
									otherwise will return the next flow in the queue
			if returns NULL -> all flows are empty */
		if (curr_flow != NULL) {
			curr_flow = schedulerStep(flow_head, curr_flow, args->step_size, time, files->output_file);
		}

		/* exit loop condition */
		if (curr_flow == NULL && no_more_inputs) {
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