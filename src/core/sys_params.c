/*
 * sys_params.c
 *
 *  Created on: 6 sty 2019
 *      Author: tolek
 */
#include "core/sys_params.h"
#include "utils/params.h"
#include "platform/storage.h"
#include <stdlib.h>

static Params* par;

void sys_init_params() {
	par = params_init();

	const char* tmp = load(0);
	params_set_all(par,tmp);
	free((void*)tmp);


}

void sys_store_params() {
	const char* tmp = params_get_all(par);
	store(0,tmp);
	free((void*)tmp);
}

void sys_params_set(const char* key, const char* value) {
	params_set(par,key,value);
}
void sys_params_set_default(const char* key, const char* value) {
	char* val = sys_params_get(key);
	if (val == NULL) {
		sys_params_set(key,value);
	}
}

char* sys_params_get(const char* key){
	return params_get(par,key);
}

