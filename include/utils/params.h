/*
 * params.h
 *
 *  Created on: 19 gru 2018
 *      Author: lukasz
 */

#ifndef UTILS_PARAMS_H_
#define UTILS_PARAMS_H_

typedef struct _ParamsPair ParamsPair;

struct _ParamsPair{
	char* key;
	char* value;
	ParamsPair* next;
};


typedef struct _Params Params;

Params* params_init();
void params_free(Params* par);

char* params_get(Params* par, const char* key);
char* params_get_all(Params* par);
void params_set_all(Params* par, const char* buff);
void params_set(Params* par, const char* key, const char* value);
ParamsPair* params_get_first(Params* par);
ParamsPair* params_get_next(Params* par, ParamsPair* curr);


#endif /* UTILS_PARAMS_H_ */
