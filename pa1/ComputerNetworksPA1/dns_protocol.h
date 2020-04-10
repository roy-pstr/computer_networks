#ifndef _DNS_PROTOCOL_H
#define _DNS_PROTOCOL_H

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

void CreateQuery(const char * name, char * dns_querie, int *len);

void ParseAnswer(const char * dns_answer, struct hostent * result);

struct hostent * dnsQuery(const char * name, const char * ip);

int FillDNSServerData(const char * ip);

int SendQuery(const char * query, int len);

int RecvAnswer(char *answer, int *recv_len);

#endif