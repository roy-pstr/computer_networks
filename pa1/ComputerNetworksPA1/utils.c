#include <stdbool.h>
#include "utils.h"

void ToLowerString(char *str)
{
	while (*str != '\0')
		*str++ = tolower(*str);
}

void PrintSyntaxError()
{
	printf("ERROR: BAD NAME\n");
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
			label_counter = 0;
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