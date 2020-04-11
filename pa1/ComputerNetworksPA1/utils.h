#ifndef _UTILS_H
#define _UTILS_H

#define MAX_IP4_STR_LEN 16
#define IP4_HEX_STR_LEN 4
#define EXPECTED_ARGS_NUM 1
#define MAX_DOMAIN_LEN 255 //maske sure
#define MAX_QUERY_LEN 512 /* check this ??? */
#define RECV_TIMEOUT 2
#define ECODE_BYTE 3
#define ID_BYTE 0
#define IP_OFFSET 4
#define DATA_LEN_OFFSET 6
#define MAX_URL_LABEL_LENGTH 63
/* From 'outputs_format.pdf'. not sure what to do with it*/
#define NO_ERROR			 0
#define FORMAT_ERROR		 1
#define SERVER_FAILURE_ERROR 2
#define NON_EXISTING_DOMAIN  3
#define NOT_IMPLEMENTED		 4
#define QUERY_REFUSED		 5
#define URL_ERROR			6
#define TIME_OUT			7
/*********************************************************/


void ToLowerString(char *str);
void printError(int e_val);

bool ValidUrlAddress(const char *url_address);

void TwoChars2Int(const char arr[2], unsigned short * val);

#endif
