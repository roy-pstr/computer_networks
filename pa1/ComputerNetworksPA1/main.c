#include <stdio.h>
#include <string.h>
#include <ctype.h> //for tolower()
#include <assert.h>
#include <stdbool.h>
#include "utils.h"
#include "dns_protocol.h"
#include <stdlib.h>


int main(int argc, char **argv)
{
	if (argc != EXPECTED_ARGS_NUM + 1) {
		perror("not enough argumnets");
		return -1;
	}

	char user_input[MAX_DOMAIN_LEN];
	while (true)
	{
		gets(user_input);
		ToLowerString(user_input);
		if (!strcmp(user_input, "quit"))
			break;
		if (!ValidUrlAddress(user_input)) {
			printError(URL_ERROR);
			continue;
		}

		struct hostent* result = dnsQuery(user_input, argv[1]);
		if (result == NULL)
		{
			continue;
		}

		IN_ADDR addr;
		addr.S_un.S_addr = *(ULONG*)result->h_addr_list[0];
		printf("%s\n\n",inet_ntoa(addr));

		freeHostentStruct(&result);
		
	}
	
	return 0;
}