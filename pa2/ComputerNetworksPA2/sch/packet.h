#ifndef _PACKET_H
#define _PACKET_H
#include <stdbool.h>
#include <stdio.h>

typedef struct packet_st
{
	int id;
	int len;
	int curr_len;
	int arrival_time; /* the time the packet arrived */
	int start_time; /* the time the packet start sending */
	int done_time; /* the time the packet finished sending */

	struct packet_st *next;
}Packet;

bool timeToRecivePacket(const char *line, int time);
void initPacket(Packet * p);
bool pcktDone(Packet * pckt);
void pcktStep(Packet *pckt, int size);
bool pcktStart(Packet * pckt);

#endif


