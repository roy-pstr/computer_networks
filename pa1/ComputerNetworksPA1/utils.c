#include <stdbool.h>
#include <stdio.h>
#include "utils.h"

void ToLowerString(char *str)
{
	while (*str != '\0')
		*str++ = tolower(*str);
}
void printError(int e_val) {
	switch (e_val)
	{
	case FORMAT_ERROR:
		printf("ERROR: FORMAT\n");
		break;
	case SERVER_FAILURE_ERROR:
		printf("ERROR: SERVER FAILURE\n");
		break;
	case NON_EXISTING_DOMAIN:
		printf("ERROR: NONEXISTING\n");
		break;
	case NOT_IMPLEMENTED:
		printf("ERROR: NOT IMPLEMENTED\n");
		break;
	case QUERY_REFUSED:
		printf("ERROR: REFUSED\n");
		break;
	case URL_ERROR:
		printf("ERROR: BAD NAME\n");
		break;
	case TIME_OUT:
		printf("TIMEOUT\n");
		break;
	default:
		break;
	}
}

bool isLetter(char c)
{
	return (c >= 'a' && c <= 'z');
}

bool isDigit(char c)
{
	return (c >= '0' && c <= '9');
}

bool StartsWithLetter(const char *str)
{
	return isLetter(*str);
}

bool EndsWithLetterOrDigit(const char *str)
{
	char last_char = str[strlen(str) - 1];
	return (isLetter(last_char) || isDigit(last_char));
}

bool ContainsForbiddenChars(const char *url_address)
{
	while (*url_address != '\0')
	{
		if (!((isLetter(*url_address)) || (isDigit(*url_address)) || (*url_address == '.') || (*url_address == '-')))
			return true;
		url_address++;
	}
	return false;
}

bool InvalidLabelLength(const char *url_address, int max_len)
{
	int label_counter = 0;
	while (*url_address != '\0')
	{
		if (*url_address == '.')
		{
			if (label_counter == 0) //case two dots in a row
				return true;
			label_counter = 0;
		}
		else
			label_counter++;

		if (label_counter > max_len)
			return true;
		url_address++;
	}
	return false;
}

bool ValidUrlAddress(const char *url_address)
{
	//this function assumes url_address is after the conversion to lowercase...
	if (!StartsWithLetter(url_address))
		return false;
	if (!EndsWithLetterOrDigit(url_address))
		return false;
	if (ContainsForbiddenChars(url_address))
		return false;
	if (InvalidLabelLength(url_address, MAX_URL_LABEL_LENGTH))
		return false;
	return true;
}

void TwoChars2Int(const char arr[2], unsigned short *val) {
	unsigned short lsb = (unsigned short)arr[1];
	unsigned short msb = ((unsigned short)arr[0]) << 8;
	*val = msb + lsb;
}