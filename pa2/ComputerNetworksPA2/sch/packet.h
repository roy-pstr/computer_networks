#ifndef _PACKET_H
#define _PACKET_H
#include <stdbool.h>

typedef struct packet_st
{
	int id;
	int len;
	int curr_len;
	int arrival_time; /* the time the packet arrived */
	int start_time; /* the time the packet start sending */
	int done_time; /* the time the packet finished sending */

	struct packet_st *next;
}packet_st;

bool pcktDone(packet_st * pckt);
void pcktStep(packet_st *pckt, int size);
bool pcktStart(packet_st * pckt);

#endif


