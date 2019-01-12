/*
 * utils.h
 *
 *  Created on: 17 gru 2018
 *      Author: lukasz
 */

#ifndef TERMINAL_UTILS_H_
#define TERMINAL_UTILS_H_

typedef struct _Splitter Splitter;

Splitter* splitter_init(const char* src, const char* separators);
char* splitter_get_item(Splitter* splt, int index);
int splitter_get_amount_items(Splitter* splt);
char** splitter_get_item_buffer(Splitter* splt);
void  splitter_free(Splitter* splt);



#endif /* TERMINAL_UTILS_H_ */
