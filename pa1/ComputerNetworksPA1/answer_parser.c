#include "answer_parser.h"
#include <stdio.h>
#include <stdbool.h>

#define DOMAIN_NAME_LENGTH_WHEN_OFFSET_MODE 2
#define OFFSET_MODE_MASK 0xc0
unsigned short HexaStringToUnsignedShort(const unsigned char *chars_arr, int num_size_in_bytes) {
	if (num_size_in_bytes == 2) {
		return (((unsigned short)chars_arr[0]) << 8) | (0x00ff & chars_arr[1]);
	}
	unsigned short lsb = (unsigned short)chars_arr[0];
	return lsb;
}

int ParseNameLength(char *domain_str) {
	int result = 0, curr_ind = 0;
	unsigned short next_lbl_len = HexaStringToUnsignedShort(&domain_str[curr_ind], 1);
	/* check if using offset */
	if ((next_lbl_len & OFFSET_MODE_MASK) == OFFSET_MODE_MASK) {
		return DOMAIN_NAME_LENGTH_WHEN_OFFSET_MODE;
	}
	while (next_lbl_len != 0) {
		curr_ind += 1 + next_lbl_len;
		next_lbl_len = HexaStringToUnsignedShort(&domain_str[curr_ind], 1);
	}
	result = curr_ind + 1;
	return result;
}

int ParseHeader(unsigned short *number_of_answers, const char *dns_answer, unsigned short match_id) {
	/* HEADER */

	/* transtion ID */
	unsigned short id = HexaStringToUnsignedShort(&dns_answer[ID], 2);
	if (htons(id) != match_id) {
		printf("Answer and Question ID are different.\n");
		return TIME_OUT;
	}

	/* ERROR_CODE */
	unsigned short errorcode = HexaStringToUnsignedShort(&dns_answer[FLAGS], 2) & 0x000f; /* last 4 bits from the FLAGS short desc. */
	if (errorcode != NO_ERROR) {
		return errorcode;
	}

	/* number of queries */
	unsigned short number_of_queries = HexaStringToUnsignedShort(&dns_answer[QUESTIONS], 2);
	if (number_of_queries > 1) {
		printf("Supports no more than 1 query (%d queries recived).", number_of_queries);
		//assert(false); /* debug */
	}

	/* number of rrs recived */
	*number_of_answers = HexaStringToUnsignedShort(&dns_answer[ANSWER_RR], 2);
	//unsigned short number_of_authurties;
	//unsigned short number_of_additional;
	return NO_ERROR;
}

void ParseQuery(int *idx, unsigned short *query_type, unsigned short *query_class, const char *dns_answer) {
	/* domain name */
	int name_length = ParseNameLength(&dns_answer[*idx]);
	/* query type */
	*query_type = HexaStringToUnsignedShort(&dns_answer[*idx + name_length], 2);
	/* query class */
	*query_class = HexaStringToUnsignedShort(&dns_answer[*idx + name_length + TYPE_SIZE], 2);

	*idx += name_length + TYPE_SIZE + CLASS_SIZE; /* move idx to start of first rr */
}

bool ParseAndFindFirstAnswer(int *idx, struct answer *output, unsigned short number_of_answers, unsigned short query_type, unsigned short query_class, const char *dns_answer) {
	
	unsigned short answer_type = 0, answer_class = 0, rdata_len = 0;
	bool found_answer = false;
	int name_length = 0;
	for (int i = 0; i < number_of_answers; i++)
	{
		/* domain name */
		name_length = ParseNameLength(&dns_answer[*idx]);
		/* answer type */
		answer_type = HexaStringToUnsignedShort(&dns_answer[*idx + name_length], 2);
		/* answer class */
		answer_class = HexaStringToUnsignedShort(&dns_answer[*idx + name_length + TYPE_SIZE], 2);
		/* RDATA length */
		rdata_len = HexaStringToUnsignedShort(&dns_answer[*idx + name_length + TYPE_SIZE + CLASS_SIZE + TTL_SIZE], 2);
		/* check if this is the answer we are looking for: */
		if (answer_type == query_type && answer_class == query_class) {
			found_answer = true;
		}

		/* RDATA */
		if (found_answer) {
			output->data_len = rdata_len;
			strncpy_s(output->ip_address, sizeof(output->ip_address), &dns_answer[*idx + name_length + TYPE_SIZE + CLASS_SIZE + TTL_SIZE + DATA_LEN_SIZE], rdata_len);
			return true;
		}
		*idx += *idx + name_length + TYPE_SIZE + CLASS_SIZE + TTL_SIZE + DATA_LEN_SIZE + rdata_len; /* move idx to start of next rr */
	}
	return false; /* didnt find an answer! */
}

int ParseAnswer(const char *dns_answer, int len, unsigned short match_id, struct answer *output) {
	int idx = 0;
	unsigned short number_of_answers = 0;
	int result = NO_ERROR;

	/* HEADER */
	if (NO_ERROR != (result = ParseHeader(&number_of_answers, dns_answer, match_id))){
		return result;
	}
	idx += HEADER_BYTES_SIZE;

	/* QUERIES */
	unsigned short query_type, query_class;
	ParseQuery(&idx, &query_type, &query_class, dns_answer);

	/* RRs */
	if (!ParseAndFindFirstAnswer(&idx, output, number_of_answers, query_type, query_class, dns_answer)) {
		return NO_ANSWER_MATH_TYPE_AND_CLASS;
	}

	return NO_ERROR;
}