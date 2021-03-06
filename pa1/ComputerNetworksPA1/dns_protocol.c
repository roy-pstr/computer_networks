
#include "stdio.h"
#include "string.h"
#include "stdlib.h"
#include "dns_protocol.h"
#include "utils.h"
#include "answer_parser.h"
#define DNS_PORT 53
#define NS 2
#define TYPE_A 1

id_counter = 0;
WSADATA wsaData;
SOCKET m_socket = INVALID_SOCKET;
SOCKADDR_IN dns_address;

void printHexString(const char *query, int len) {
	for (size_t i = 0; i < len; i++)
	{
		printf("%02x ", (unsigned char)query[i]);
	}
	printf("\n");
}
int fillDNSServerData(const char *ip) {
	unsigned long address;
	address = inet_addr(ip);
	if (address == INADDR_NONE)
	{
		printf("The string \"%s\" cannot be converted into an ip address.\n", ip);
		return IP_ERROR; /*TODO: ERROR CODE*/
	}
	dns_address.sin_family = AF_INET;
	dns_address.sin_addr.s_addr = address;
	dns_address.sin_port = htons(DNS_PORT); //Setting the port.
	return NO_ERROR;
}
 int allocateHostent(struct hostent **ptr) {
	 *ptr = malloc(sizeof(struct hostent));
	if (NULL == *ptr) {
		perror("malloc error.\n");
		return 1;
	}
	/* handling only one address */
	(*ptr)->h_addr_list = malloc(2 * sizeof(char*));
	if (NULL == (*ptr)->h_addr_list) {
		perror("malloc error.\n");
		freeHostentStruct(ptr);
		return 1;
	}
	(*ptr)->h_addr_list[0] = malloc(5);
	if (NULL == (*ptr)->h_addr_list[0]) {
		perror("malloc error.\n");
		freeHostentStruct(ptr);
		return 1;
	}
	(*ptr)->h_addr_list[1] = NULL;
	return 0;
}
void freeHostentStruct(struct hostent **result) {
	if (NULL != (*result)->h_addr_list) {
		char *curr = NULL, *next = NULL;
		int ind = 0;
		curr = (*result)->h_addr_list[ind];
		while (NULL != curr) {
			next = (*result)->h_addr_list[++ind];
			if (NULL != curr) {
				free(curr);
				curr = NULL;
			}
			if (NULL != next) {
				curr = next;
			}
		}
	}
	if (NULL != *result) {
		free(*result);
		*result = NULL;
	}

}

/* Query handling */
int SendQuery(char * query, int len) {
	/* Send the Query (the sendto function handles also the connect!)*/
	printf("Sending a query to the DNS server...\n"); /*debug*/
	if (SOCKET_ERROR == sendto(m_socket, (char *)query, len, 0, (SOCKADDR *)&dns_address, sizeof(dns_address))) {
		perror("sendto() failed\n");
		return 1; /* TODO handle error */
	}
	return 0;
}

void mem_copy(void *dest, const void *source, int size)
{
	char *d = (char*)dest;
	char *s = (char*)source;
	for (int i = 0; i < size; i++)
		d[i] = s[i];
}

void EnableRD(struct dns_header *header)
{
	header->options |= htons(0x0100);
}

void CreateHeader(struct dns_header *header)
{
	header->id = htons(id_counter);
	id_counter++; //TBD make sure it belongs in here
	header->options = 0;
	EnableRD(header);
	header->qdcount = htons(1);
	header->ancount = 0;
	header->nscount = 0;
	header->arcount = 0;	
}

void Print_n_chars(const char *str, int n)
{
	for (int i = 0; i < n; i++)
		printf("%c", str[i]);
}

void PrintDomainName(const char *domain_name) //this function is not needed...
{
	bool first_iteration = true;
	while (true)
	{
		int num_of_chars_to_print = *domain_name;
		if (num_of_chars_to_print == 0)
			break;
		if (!first_iteration)
			printf(".");
		domain_name++;
		Print_n_chars(domain_name, num_of_chars_to_print);
		domain_name += num_of_chars_to_print;
		first_iteration = false;
	}
	printf("\n");
}

char CountNumOfCharsBeforeDot(const char *url_address, bool *end_of_string)
{
	char counter = 0;
	while (true) {
		if (*url_address == '\0') {
			*end_of_string = true;
			break;
		}
		if (*url_address == '.')
			break;
		url_address++;
		counter++;
	}
	return counter;
}

void CreateDomainName(const char *url_address, char *domain_name)
{
	bool end_of_string = false;
	while (!end_of_string)
	{
		char num_of_chars = CountNumOfCharsBeforeDot(url_address, &end_of_string);
		*domain_name++ = num_of_chars;
		for (int i = 0; i < num_of_chars; i++)
			*domain_name++ = *url_address++;
		url_address++;
	}
	*domain_name = 0;
}

void CreateQuestion(struct question *quest)
{
	quest->qclass = htons(1); //internet
	quest->qtype = htons(TYPE_A);
}

unsigned short CreateQuery(const char *url_address, char **query, int *len) {
	//allocates memory for query!!!!
	*len = sizeof(struct dns_header) + strlen(url_address) + 2 + sizeof(struct question);
	*query = (char*)malloc(*len);
	if (NULL == *query) {
		perror("malloc error.\n");
		return 0;
	}
	struct dns_header header;
	CreateHeader(&header);
	struct question quest;
	CreateQuestion(&quest);
	mem_copy(*query, &header, sizeof(struct dns_header));
	CreateDomainName(url_address, *query + sizeof(struct dns_header));
	mem_copy(*query + *len - sizeof(struct question), &quest, sizeof(struct question));
	return header.id;
}

/* Answer handling */
int RecvAnswer(char *answer, int *recv_len) {
	fd_set set;
	struct timeval timeout;
	FD_ZERO(&set); /* clear the set */
	FD_SET(m_socket, &set); /* add our file descriptor to the set */
	timeout.tv_sec = RECV_TIMEOUT;
	timeout.tv_usec = 0;
	int retval_select = 1;
	printf("Waiting for an answer from DNS server...\n");/*debug*/
	retval_select = select(0, &set, NULL, NULL, &timeout);
	if (retval_select == SOCKET_ERROR) {
		// select error...
		perror("select() failed\n");
		return 1;
	}
	if (retval_select == 0) {
		// timeout
		printError(TIME_OUT);
		return 1;
	}

	*recv_len = recvfrom(m_socket, answer, MAX_QUERY_LEN, 0, NULL, NULL);
	if (SOCKET_ERROR == *recv_len) {
		printf("recvfrom failed with error %d\n", WSAGetLastError());
		perror("recvfrom() failed\n");
		return 1;
	}
	//printf("Answer recived:\n"); /*debug*/
	//printHexString(answer, *recv_len); /*debug*/
	//printHexString(answer_example, 51); /*debug*/
	/* Close socket */
	if (m_socket != INVALID_SOCKET) {
		if (SOCKET_ERROR == closesocket(m_socket)) {
			printf("closesocket failed with error: %d\n", WSAGetLastError());
		}
		m_socket = INVALID_SOCKET;
	}
	return 0;
}

void FillHostent(struct hostent *result, struct answer *answer_st) {
	/* ip address */
	strncpy_s(result->h_addr_list[0], IP4_HEX_STR_LEN + 1, answer_st->ip_address, IP4_HEX_STR_LEN);
	result->h_addrtype = AF_INET;
	result->h_length = answer_st->data_len;
}

/* dnsQeury */
struct hostent * dnsQuery(const char * name, const char* ip)
{
	char* dns_query = NULL;

	/* Initialize Winsock: */
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (StartupRes != NO_ERROR)
	{
		printf("error %ld at WSAStartup( ).\n", WSAGetLastError());
		return NULL;
	}

	/* allocate hostent struct */
	struct hostent *result;
	if (1 == allocateHostent(&result)) {
		goto EXIT_WITH_ERROR;
	}

	/* Create the DNS query */
	int query_len;
	unsigned short q_id;
	q_id = CreateQuery(name, &dns_query, &query_len);
	if (NULL == dns_query) {
		goto EXIT_WITH_ERROR;
	}
	/* Create DNS server address data struct - sockaddr_in */
	if (NO_ERROR != fillDNSServerData(ip)) {
		goto EXIT_WITH_ERROR;
	}

	/* Create socket */
	if (INVALID_SOCKET == (m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))) {
		printf("socket failed with error %d\n", WSAGetLastError());
		goto EXIT_WITH_ERROR;
	}

	/* Send the Query to the DNS server */
	if (1 == SendQuery(dns_query, query_len)) {
		goto EXIT_WITH_ERROR;
	}
	

	/* Wait 2 seconds to recive the answer from the DNS server */
	char dns_answer[MAX_QUERY_LEN];
	int answer_len;
	if (1 == RecvAnswer(dns_answer, &answer_len)) {
		goto EXIT_WITH_ERROR;
	}

	/*Parse the DNS server answer and fill the fields of the hostent struct. */
	//ParseAnswer(dns_answer, answer_len, result);
	struct answer answer_st;
	int ret_val;
	if (NOERROR != (ret_val = ParseAnswer(dns_answer, answer_len, q_id, &answer_st))) {
		printError(ret_val);
		goto EXIT_WITH_ERROR;
	}
	else {
		FillHostent(result, &answer_st);
	}


EXIT:
	/* Close socket */
	if (m_socket != INVALID_SOCKET) {
		if (SOCKET_ERROR == closesocket(m_socket)) {
			printf("closesocket failed with error: %d\n", WSAGetLastError());
		}
		m_socket = INVALID_SOCKET;
	}

	/* Winsock Cleanup: */
	if (WSACleanup() == SOCKET_ERROR)
		printf("Failed to close Winsocket, error %ld. Ending program.\n", WSAGetLastError());

	return result;

EXIT_WITH_ERROR:
	freeHostentStruct(&result);
	if (dns_query != NULL) {
		free(dns_query);
	}
	goto EXIT;
}

