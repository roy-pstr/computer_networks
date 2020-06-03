#ifndef _PACKET_H
#define _PACKET_H
#include <stdbool.h>
typedef struct packet_st
{
	int id;
	int curr_len;
	packet_st *next;
}packet_st;

bool pcktDone(packet_st * pckt);
void pcktStep(packet_st *pckt, int size);

#endif


