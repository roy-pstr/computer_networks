#include "packet.h"
#include <assert.h>
#include "utils.h"
#include "string.h"

void initPacket(Packet *p) {
	p->arrival_time = 0;
	p->curr_len = 0;
	p->done_time = 0;
	p->id = 0;
	p->len = 0;
	p->next = NULL;
	p->start_time = 0;
}

bool pcktDone(Packet *pckt, int quant_size) {
	if (quant_size == 0) {
		return (pckt->curr_len == 0);
	}
	else {
		return ((pckt->len - pckt->curr_len) % quant_size) == 0;
	}
}

void pcktStep(Packet *pckt, int size){

	pckt->curr_len -= size;
	if (pckt->curr_len < 0) { pckt->curr_len = 0; }
}

bool pcktStart(Packet *pckt) {
	return (pckt->curr_len == pckt->len);

}

bool timeToRecivePacket(const char *line, int curr_time)
{
	//char *time_pointer = getPointerAfterSpace(line);
	char *time_pointer = getPointerAfterSpace(line);
	//while (*time_pointer != ' ') {
	//	time_pointer++;
	//}
	//time_pointer++;
	int time = atoi(time_pointer);
	assert(curr_time <= time);
	return curr_time == time;
}