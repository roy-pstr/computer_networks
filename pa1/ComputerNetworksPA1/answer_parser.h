#ifndef _ANSWER_PARSER_H
#define _ANSWER_PARSER_H

#include "utils.h"
enum header_idx {
	ID = 0,
	FLAGS = 2,
	QUESTIONS = 4,
	ANSWER_RR = 6,
	AUTH_RR = 8,
	ADDITIONAL_RR = 10,
	HEADER_BYTES_SIZE = 12
};

enum question_and_rr_idx {
	RR_NAME = 0,
	TYPE_SIZE = 2,
	CLASS_SIZE = 2,
	TTL_SIZE = 4,
	DATA_LEN_SIZE = 2,
};

typedef struct answer
{
	unsigned short id;
	unsigned short errorcode;
	char ip_address[IP4_HEX_STR_LEN + 1];
	unsigned short data_len;

}answer;

unsigned short HexaStringToUnsignedShort(const char * chars_arr, int num_size_in_bytes);

int ParseAnswer(const char * dns_answer, int len, unsigned short match_id, answer * output);

#endif


