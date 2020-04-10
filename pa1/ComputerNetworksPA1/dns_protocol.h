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
	char domain_name[MAX_DOMAIN_LEN];
	unsigned short qtype;
	unsigned short qclass;	
};

void CreateHeader(struct dns_header * header);

char CountNumOfCharsBeforeDot(const char * url_address, bool * end_of_string);

void CreateDomainName(const char * url_address, char * domain_name);

void CreateQuery(const char *url_address, struct question *quest);

void ParseAnswer(const char *dns_answer, int len, struct hostent *result);

struct hostent * dnsQuery(const char * name, const char * ip);

int FillDNSServerData(const char * ip);

int SendQuery(struct question * quest, int len);

int RecvAnswer(char *answer, int *recv_len);

#endif