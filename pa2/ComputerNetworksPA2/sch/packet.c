#include "packet.h"
#include <assert.h>

bool pcktDone(packet_st *pckt) {
	return (pckt->curr_len == 0);
}

void pcktStep(packet_st *pckt, int size){

	pckt->curr_len -= size;
	if (pckt->curr_len < 0) { pckt->curr_len = 0; }
}

bool pcktStart(packet_st *pckt) {
	return (pckt->curr_len == pckt->len);

}

bool timeToSendPacket(const char *line, int curr_time)
{
	char *time_pointer = getPointerAfterSpace(line);
	int time = atoi(time_pointer);
	assert(curr_time <= time);
	return curr_time == time;
}