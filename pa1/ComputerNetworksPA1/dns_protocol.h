#ifndef _DNS_PROTOCOL_H
#define _DNS_PROTOCOL_H

#include <winsock2.h>
#pragma comment(lib, "ws2_32.lib")

#define DOMAIN_NAME_MAX_LEN 

typedef struct dns_header
{
	unsigned short id;
	unsigned char qr : 1;
	unsigned char opcode : 4;
	unsigned char aa : 1;
	unsigned char tc : 1;
	unsigned char rd : 1;
	unsigned char ra : 1;
	unsigned char z : 3;
	unsigned char rcode : 4;
	unsigned short qdcount;
	unsigned short ancount;
	unsigned short nscount;
	unsigned short arcount;
};
typedef struct question
{
	struct dns_header header;
	unsigned short qtype;
	unsigned short qclass;

	char domain_name[DOMAIN_NAME_MAX_LEN];
	
};

void CreateQuery(const char * name, char * dns_querie, int *len);

void ParseAnswer(const char * dns_answer, struct hostent * result);

struct hostent * dnsQuery(const char * name, const char * ip);

int FillDNSServerData(const char * ip);

int SendQuery(const char * query, int len);

int RecvAnswer(char *answer, int *recv_len);

#endif