#ifndef _DNS_PROTOCOL_H
#define _DNS_PROTOCOL_H

#include <stdlib.h>
#include <stdbool.h>
#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")
#include "utils.h"

typedef struct dns_header
{
	unsigned short id;
	unsigned short options;
	unsigned short qdcount;
	unsigned short ancount;
	unsigned short nscount;
	unsigned short arcount;
}dns_header;

typedef struct question
{
	unsigned short qtype;
	unsigned short qclass;	
}question;

struct hostent * dnsQuery(const char * name, const char * ip);

void freeHostentStruct(struct hostent ** result);

#endif