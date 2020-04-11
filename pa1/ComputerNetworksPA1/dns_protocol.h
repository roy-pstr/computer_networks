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
}dns_header;

typedef struct question
{
	unsigned short qtype;
	unsigned short qclass;	
}question;

typedef struct answer
{
	unsigned short id;
	unsigned short errorcode;
	char ip_address[IP4_HEX_STR_LEN + 1];
	unsigned short data_len;

}answer;

void CreateHeader(struct dns_header * header);

char CountNumOfCharsBeforeDot(const char * url_address, bool * end_of_string);

void CreateDomainName(const char * url_address, char * domain_name);

unsigned short CreateQuery(const char *url_address, char **query, int *len);

struct hostent * dnsQuery(const char * name, const char * ip);

struct hostent * dnsQueryTest(const char * name, const char * ip);

freeHostentStruct(struct hostent ** result);

int FillDNSServerData(const char * ip);

int SendQuery(char * query, int len);

int RecvAnswer(char *answer, int *recv_len);

int ValidateAnswer(struct answer * answer_st, unsigned short q_id);

void ParseAnswer(const char * dns_answer, int len, struct answer * output);

void FillHostent(struct hostent * result, struct answer * answer_st);

#endif