#include "dns_protocol.h"
#include "utils.h"

#define DNS_PORT 53
#define NS 2

WSADATA wsaData;
SOCKET m_socket = INVALID_SOCKET;
SOCKADDR_IN dns_address;

const char query_example[35] = "\x00\x0c\x01\x00\x00\x01\x00\x00\x00\x00\x00\x00\x03\x77\x77\x77\x08\x61\x63\x61\x64\x65\x6d\x69\x61\x03\x65\x64\x75\x00\x00\x01\x00\x01";
void printHexString(const char *query, int len) {
	for (size_t i = 0; i < len; i++)
	{
		printf("%02x", (unsigned char)query[i]);
	}
	printf("\n");
}


void CreateHeader(struct dns_header *header)
{
	header->id = (unsigned short)htons(getpid());
	header->qr = 0; //for query
	header->opcode = 0;
	header->aa = 0;
	header->tc = 0;
	header->rd = htons(1); 
	header->ra = 0; 
	header->z = 0;
	header->rcode = 0;
	header->qdcount = htons(1);
	header->ancount = 0;
	header->nscount = 0;
	header->arcount = 0;	
}


void CreateQuery(const char *url_address, struct question *quest) {
	CreateHeader(&(quest->header));
	CreateDomainNAme(url_address, quest->domain_name);
	quest->qclass = htnos(1); //internet
	quest->qtype = htons(NS);
}

void ParseAnswer(const char *dns_answer, int len, struct hostent *result) {
	char ip_address[IP_MAX_LEN];
	printf("%s\n", dns_answer);
	for (size_t i = 0; i < 4; i++)
	{
		printf("%02x", (unsigned char)dns_answer[i]);
	}
	for (size_t i = 0; i < 4; i++)
	{
		printf("%c", (unsigned char)dns_answer[i]);
	}
	//result->h_addr_list[0] = 
}

struct hostent * dnsQuery(const char * name, const char* ip)
{
	struct hostent* result = NULL;

	/* Initialize Winsock: */
	int StartupRes = WSAStartup(MAKEWORD(2, 2), &wsaData);
	//if (StartupRes != NO_ERROR)
	//{
	//	printf("error %ld at WSAStartup( ), ending program.\n", WSAGetLastError());
	//	return result;
	//}

	/* allocate hostent struct */
	result = (struct hostent*)malloc(sizeof(struct hostent));
	//if (NULL == result) {
	//	return result;
	//}

	/* Create the DNS query */
	struct question dns_querie;
	CreateQuery(name, &dns_querie);

	/* Create DNS server address data struct - sockaddr_in */
	FillDNSServerData(ip);

	/* Create socket */
	if (INVALID_SOCKET == (m_socket = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))) {
		printf("socket failed with error %d\n", WSAGetLastError());
		return 1; /*TODO: ERROR CODE*/
	}

	/* Send the Query to the DNS server */
	if (1 == SendQuery(query_example, 35)) {
		/* TODO handle error */
		printf("SendQuery() failed.\n");
	}

	/* Wait 2 seconds to recive the answer from the DNS server */
	char dns_answer[MAX_QUERY_SIZE];
	int answer_len;
	if (1 == RecvAnswer(dns_answer, &answer_len)) {
		/* TODO handle error */
		printf("RecvAnswer() failed.\n");
	}

	/*Parse the DNS server answer and fill the fields of the hostent struct. */
	ParseAnswer(dns_answer, answer_len, result);
	

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
}

int FillDNSServerData(const char *ip) {
	unsigned long address;
	address = inet_addr(ip);
	if (address == INADDR_NONE)
	{
		printf("The string \"%s\" cannot be converted into an ip address. ending program.\n", ip);
		return NULL; /*TODO: ERROR CODE*/
	}
	dns_address.sin_family = AF_INET;
	dns_address.sin_addr.s_addr = address;
	dns_address.sin_port = htons(DNS_PORT); //Setting the port.
}

int SendQuery(const char *query, int len) {
	/* Send the Querie (the sendto function handles also the connect!)*/
	printf("Sending a query to the DNS server...\n");
	if (SOCKET_ERROR == sendto(m_socket, query, len, 0, (SOCKADDR *)&dns_address,sizeof(dns_address))) {
		printf("sendto failed with error: %d\n", WSAGetLastError());
		//perror();
		return 1; /* TODO handle error */
	}
	printf("Query sent:\n");
	printHexString(query, len); /*debug*/
	return 0;
}

int RecvAnswer(char *answer, int *recv_len) {
	fd_set set;
	struct timeval timeout;
	FD_ZERO(&set); /* clear the set */
	FD_SET(m_socket, &set); /* add our file descriptor to the set */
	timeout.tv_sec = RECV_TIMEOUT;
	timeout.tv_usec = 0;
	int retval_select = 1;
	printf("Waiting for an answer from DNS server...\n");
	retval_select = select(0, &set, NULL, NULL, &timeout);
	if (retval_select == SOCKET_ERROR) {
		// select error...
		printf("select() failed with error: %d\n", WSAGetLastError());
		return 1;
	}
	if (retval_select == 0) {
		// timeout
		printf("recvfrom() tiemout.\n");
		return 1;
	}

	*recv_len = recvfrom(m_socket, answer, MAX_QUERY_SIZE, 0, NULL, NULL);
	if (SOCKET_ERROR == *recv_len) {
		printf("recvfrom failed with error %d\n", WSAGetLastError());
		return 1;
	}
	printf("Answer recived:\n");
	printHexString(answer, *recv_len);
	/* Close socket */
	if (m_socket != INVALID_SOCKET) {
		if (SOCKET_ERROR == closesocket(m_socket)) {
			printf("closesocket failed with error: %d\n", WSAGetLastError());
		}
		m_socket = INVALID_SOCKET;
	}
	return 0;
}

