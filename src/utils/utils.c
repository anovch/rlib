/*
 * utils.c
 *
 *  Created on: 17 gru 2018
 *      Author: lukasz
 */
#include <stddef.h>
#include <string.h>
#include <stdlib.h>
#include "utils/utils.h"

#define MAX_ITEMS_IN_SPLITTER 16
struct _Splitter{
	char* items[MAX_ITEMS_IN_SPLITTER];
	char* buff_;
	int index;
};


static void set_delim(char* buff, char delim, int len) {
	for (int i=0;i<len;i++) {
		if (buff[i] == delim) {
			buff[i] = 0;
		}
	}
}

static char* get_first_tok(char* buff,  char* end) {
	while(*buff==0) {
		buff++;
		if (buff == end) {
			return NULL;
		}
	}
	return buff;
}

static char* get_next_tok(char* buff,  char* end) {
	while(*buff!=0) {
		buff++;
		if (buff == end) {
			return NULL;
		}
	}
	while(*buff==0) {
		buff++;
		if (buff == end) {
			return NULL;
		}
	}

	return buff;
}

Splitter* splitter_init(const char* src, const char* separators) {

	Splitter* result;
	result = (Splitter*)malloc(sizeof(Splitter));
	memset((void*)result,0,sizeof(Splitter));


	int size = strlen(src);
	result->buff_ = malloc(size+1);
	result->buff_[size] = 0;
	memcpy(result->buff_,src,size);

	for (int i=0;i<MAX_ITEMS_IN_SPLITTER;i++) {
		result->items[i] = NULL;
	}

	for (int i=0;i<strlen(separators);i++) {
		set_delim(result->buff_,separators[i], size);
	}

	result->index = 0;
	char* ch = get_first_tok(result->buff_,&result->buff_[size]);
	while(ch!=NULL) {
		if (result->index>=MAX_ITEMS_IN_SPLITTER) {
			break;
		}
		result->items[result->index] = ch;
		ch = get_next_tok(ch,&result->buff_[size]);
		result->index++;
	}


	return result;
}

char** splitter_get_item_buffer(Splitter* splt) {
	return splt->items;
}

char* splitter_get_item(Splitter* splt, int index) {
	if (index<MAX_ITEMS_IN_SPLITTER) {
		return splt->items[index];
	}
	return NULL;
}

int splitter_get_amount_items(Splitter* splt) {
	return splt->index;
}

void  splitter_free(Splitter* splt) {
	free(splt->buff_);
	free(splt);
}

