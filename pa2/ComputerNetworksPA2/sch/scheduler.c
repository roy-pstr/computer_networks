#include <stdlib.h>
#include <stdbool.h>
#include "utills.h"
#include "scheduler.h"
#include "packet.h"
#include "flow.h"


void runScheduler(int type, int step_size) {
	flow_st flows_stack[FLOW_MAX_NUM];
	flow_st *curr_flow;
	int time = 0;
	bool flows_stack_not_empty = true;
	
	while (flows_stack_not_empty) {
		
		/* can handle more than 1 packet per time unit! */
		if (packetsInput()) { 
			storePackets();
		}

		/* sends the next part of the packet in the current flow (WRR -> Byte; DRR -> Size of Bytes) per time unit */
		enum PACKET_STATUS pckt_status;
		pckt_status = flowStep(curr_flow, step_size);
		switch (pckt_status)
		{
		case SAME_PACKET:
			break;
		case NEXT_PACKET:
			/*  proceeds to the next packet.
			could be in the same flow or next flow.
			if last flow, go back to the first flow.
			if last packet in flow, remove flow from stack.
			*/
			curr_flow = nextPacket(curr_flow);
			break;
		case FAILED:
			break;
		default:
			break;
		}

		if (curr_flow == NULL) {
			/* finish sending all the packets of all the flows */
			flows_stack_not_empty = false;
		}
		time++;
	}
}