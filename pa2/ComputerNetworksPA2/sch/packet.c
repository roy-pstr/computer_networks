#include "packet.h"

bool pcktDone(packet_st *pckt) {
	return (pckt->curr_len == 0);
}

void pcktStep(packet_st *pckt, int size){
	pckt->curr_len -= size;
	if (pckt->curr_len < 0) { pckt->curr_len = 0; }
}