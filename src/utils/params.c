/*
 * params.c
 *
 *  Created on: 19 gru 2018
 *      Author: lukasz
 */

#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <utils/params.h>
#include "platform/mem.h"

#define PARAMS_MAX_SIZE_KEY 16
#define PARAMS_MAX_SIZE_VALUE 256

struct _Params{
	ParamsPair* first;
	ParamsPair* last;
	char key[PARAMS_MAX_SIZE_KEY];
	char value[PARAMS_MAX_SIZE_VALUE];

};


Params* params_init() {
	Params* result;
	result = (Params*)port_malloc(sizeof(Params));
	result->first = NULL;
	result->last = NULL;
	return result;
}

static void params_clear(Params* par) {
	ParamsPair* tmp = par->first;
	ParamsPair* next;
	while(tmp!=NULL) {
		next = tmp->next;
		port_free(tmp->key);
		port_free(tmp->value);
		port_free(tmp);
		tmp = next;
	}
	par->first = NULL;
	par->last = NULL;

}

static ParamsPair* _params_get(Params* par, const char* key) {
	ParamsPair* tmp = par->first;
	while(tmp!=NULL) {
		if (strcmp(key, tmp->key)==0) {
			return tmp;
		}
		tmp = tmp->next;
	}
	return NULL;
}


static size_t _params_calculate_size(Params* par, int *items) {
	*items = 0;
	size_t result = 0;
	ParamsPair* tmp = par->first;
	while(tmp!=NULL) {
		result += strlen(tmp->key);
		result += strlen(tmp->value);
		(*items)++;
		tmp = tmp->next;
	}
	return result;
}

char* params_get(Params* par, const char* key) {
	ParamsPair* tmp = _params_get(par, key);
	if(tmp!=NULL) {
		return tmp->value;
	}
	return NULL;
}

char* params_get_all(Params* par) {
	int items;
	char* result = NULL;
	size_t size = _params_calculate_size(par, &items);
	size_t alloc_size = size + (items*2) + 1;

	result = port_malloc(alloc_size);
	result[0] = 0;
	result[alloc_size-1] = 0;

	ParamsPair* tmp = par->first;
	while(tmp!=NULL) {
		strcat(result, tmp->key);
		strcat(result, "=");
		strcat(result, tmp->value);
		strcat(result, "|");
		tmp = tmp->next;
	}

	return result;
}

const char* get_tail(char* src, const char* dst, char terminator) {
	while(1) {
		if ((*dst)==0) {
			return dst;
		}
		if (*dst == terminator) {
			dst++;
			return dst;
		}
		*src = *dst;
		dst++;
		src++;
	}
}

void params_set_all(Params* par, const char* buff) {
	params_clear(par);
	const char* tmp = buff;
	while(1) {
		memset(par->key,0,PARAMS_MAX_SIZE_KEY);
		memset(par->value,0,PARAMS_MAX_SIZE_VALUE);
		tmp = get_tail(par->key, tmp, '=');
		if (*tmp==0) {
			break;
		}
		tmp = get_tail(par->value, tmp, '|');

		if ((strlen(par->key)!=0) && strlen(par->value)!=0) {
			params_set(par,par->key,par->value);
		}
		if (*tmp==0) {
			break;
		}
	}

}


ParamsPair* params_get_first(Params* par) {
	return par->first;
}

ParamsPair* params_get_next(Params* par, ParamsPair* curr) {
	return curr->next;
}


void params_set(Params* par, const char* key, const char* value) {
	ParamsPair* tmp = _params_get(par, key);
	if(tmp!=NULL) {
		port_free(tmp->value);
		tmp->value = port_malloc(strlen(value)+1);
		strcpy(tmp->value, value);
		return;
	}


	tmp = (ParamsPair*)port_malloc(sizeof(ParamsPair));
	tmp->key = port_malloc(strlen(key)+1);
	strcpy(tmp->key, key);
	tmp->value = port_malloc(strlen(value)+1);
	strcpy(tmp->value, value);
	tmp->next = NULL;

	if (par->last == NULL) {
		par->last = tmp;
		par->first = tmp;
	}
	else {
		par->last->next = tmp;
		par->last = tmp;
	}

}

void params_free(Params* par) {
	params_clear(par);
	port_free(par);
}
