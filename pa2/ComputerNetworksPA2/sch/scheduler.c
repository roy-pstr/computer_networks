#include <stdio.h>
#include <stdbool.h>
#include "utills.h"
#include "scheduler.h"
#include "packet.h"
#include "flow.h"


flow_st *schedulerStep(flow_st *flow) {
	if (flow->curr_num_of_steps_done == flow->weight) {
		return nextFlow(flow);
	}
	if (flowEmpty(flow)) {
		return nextFlow(flow);
	}
	return flow;
}

void runScheduler(int type, int step_size, FILE *input_file, FILE *output_file, FILE *stats_file) {
	flow_st *flow_head = NULL;
	flow_st *curr_flow = NULL;
	int time = 0;
	bool flows_stack_empty = false;
	bool no_more_inputs = false;

	/* loop over time: every iter is one time unit step */
	while (!flows_stack_empty) {
		
		/* every iter: check if any packets arrives in this time unit */
		
		if (packetsInput(input_file, time)) {
			/* if any packet arrived, store them in the flow stack. */
			/* can handle more than 1 packet per time unit! */
			storePackets(input_file, time, flow_head);
		}

		/* check if there are more packets in the input file */
		if (endOfFile(input_file)) {
			no_more_inputs = true;
		}

		/* sends the next part of the packet per time unit */
		/* if the packet fully sent, will move the flows' packts pointer to the next packet */
		packetStep(curr_flow, step_size);
		/*  proceeds to the next packet that need to be sent.
			could be a packet in the same flow or packe tin the next flow.
			if last flow, will go back to the first flow and start a new round */
		curr_flow = schedulerStep(curr_flow);

		if (curr_flow == NULL && no_more_inputs) {
			/* finished send all the packets */
			flows_stack_empty = true;
		}
		time++; /* time unit step */
	}

	cleanMem();
}