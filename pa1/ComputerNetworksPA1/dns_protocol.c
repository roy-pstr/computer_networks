#include "dns_protocol.h"
#include "utils.h"

#define DNS_PORT 53

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

void CreateQuery(const char * name, char *dns_querie, int *len) {
	/* TO DO */
}

void ParseAnswer(const char *dns_answer, struct hostent *result) {
	/* TO DO */
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
	char *dns_querie = NULL;
	int querie_len = 0; /*length of querie in Bytes*/
	CreateQuery(name, dns_querie, &querie_len);

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
	char dns_answer[ANSWER_MAX_SIZE];
	if (1 == RecvAnswer(&dns_answer[0], ANSWER_MAX_SIZE)) {
		/* TODO handle error */
		printf("RecvAnswer() failed.\n");
	}

	/*Parse the DNS server answer and fill the fields of the hostent struct. */
	ParseAnswer(dns_answer, result);
	

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

int RecvAnswer(char *answer, int len) {
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

	int answer_len;
	if (SOCKET_ERROR == (answer_len=recvfrom(m_socket, answer, len, 0, NULL, NULL))) {
		printf("recvfrom failed with error %d\n", WSAGetLastError());
		return 1;
	}
	printf("Answer recived:\n");
	printHexString(answer, answer_len);
	/* Close socket */
	if (m_socket != INVALID_SOCKET) {
		if (SOCKET_ERROR == closesocket(m_socket)) {
			printf("closesocket failed with error: %d\n", WSAGetLastError());
		}
		m_socket = INVALID_SOCKET;
	}
	return 0;
}

