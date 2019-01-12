/*
 * sys_params.h
 *
 *  Created on: 6 sty 2019
 *      Author: tolek
 */

#ifndef CORE_SYS_PARAMS_H_
#define CORE_SYS_PARAMS_H_

void sys_init_params();
void sys_store_params();
void sys_params_set(const char* key, const char* value);
void sys_params_set_default(const char* key, const char* value);
char* sys_params_get(const char* key);

#endif /* CORE_SYS_PARAMS_H_ */
