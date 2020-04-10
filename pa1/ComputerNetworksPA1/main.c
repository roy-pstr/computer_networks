#include <stdio.h>
#include <string.h>
#include <ctype.h> //for tolower()
#include <assert.h>
#include <stdbool.h>
#include "utils.h"
#include "dns_protocol.h"
#include <stdlib.h>


void ToLowerString(char *str)
{
	while (*str != '\0')
		*str++ = tolower(*str);
}

void PrintSyntaxError()
{
	printf("ERROR: BAD NAME\n");
}

bool VerifyUserInput(const char *input)
{
	return false;
}

int main(int argc, char **argv)
{
	assert(argc == EXPECTED_ARGS_NUM + 1);

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

		//struct hostent* result = gethostbyname(user_input);
		struct hostent* result = dnsQuery(user_input, argv[1]);
		if (result == NULL)
		{
			printf("NULL result\n"); //TBR
			//checkwhattodo
		}

		//IN_ADDR addr;
		//addr.S_un.S_addr = *(ULONG*)result->h_addr_list[0];
		/*printf("%s\n",inet_ntoa(addr));*/
	}
	
	return 0;
}