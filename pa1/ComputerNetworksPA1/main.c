#include <stdio.h>
#include <string.h>
#include <ctype.h> //for tolower()

#define _WINSOCK_DEPRECATED_NO_WARNINGS


/***********************/
//#define WIN32_LEAN_AND_MEAN
//#include <winsock2.h>
#include <ws2tcpip.h>

// Need to link with Ws2_32.lib
#pragma comment(lib, "ws2_32.lib")
/***********************/

#include <assert.h>
#include <stdbool.h>

#define EXPECTED_ARGS_NUM 1

#define MAX_DOMAIN_LEN 255 //maske sure

/* From 'outputs_format.pdf'. not sure what to do with it*/
//#define NO_ERROR			 0
#define FORMAT_ERROR		 1
#define SERVER_FAILURE_ERROR 2
#define NON_EXISTING_DOMAIN  3
#define NOT_IMPLEMENTED		 4
#define QUERY_REFUSED		 5
/*********************************************************/

void ToLowerString(char *str)
{
	while (*str != '\0')
		*str++ = tolower(*str);
}

void PrintSyntaxError()
{
	printf("ERROR: BAD NAME\n");
}

void Print_n_chars(const char *str, int n)
{
	for (int i = 0; i < n; i++)
		printf("%c", str[i]);
}

void PrintDomainName(const char *domain_name)
{
	while (true)
	{
		int num_of_chars_to_print = *domain_name;
		if (num_of_chars_to_print == 0)
			break;
		domain_name++;
		Print_n_chars(domain_name, num_of_chars_to_print);
		printf(".");
		domain_name += num_of_chars_to_print;
	}
	printf("\n");
}

bool VerifyUserInput(const char *input)
{
	return false;
}

int main(int argc, char **argv)
{
	assert(argc == EXPECTED_ARGS_NUM + 1);
	WSADATA wsaData;
	int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) { /* use perror? */
		printf("WSAStartup failed: %d\n", iResult);
		return 1;
	}

	char user_input[MAX_DOMAIN_LEN];
	while (true)
	{
		gets(user_input);
		ToLowerString(user_input);
		if (!strcmp(user_input, "quit"))
			break;
		if (VerifyUserInput(user_input)) {
			PrintSyntaxError();
			continue;
		}

		struct hostent* result = gethostbyname(user_input);
		if (result == NULL)
		{
			printf("NULL result\n"); //TBR
			//checkwhattodo
		}

		IN_ADDR addr;
		addr.S_un.S_addr = *(ULONG*)result->h_addr_list[0];
		printf("%s\n",inet_ntoa(addr));
	}
	
	WSACleanup();
	return 0;
}