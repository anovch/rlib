/*
 * buffer.h
 *
 *  Created on: 15 gru 2018
 *      Author: lukasz
 */

#ifndef TERMINAL_BUFFER_H_
#define TERMINAL_BUFFER_H_
#include <stddef.h>

typedef struct _BufferOutput BufferOutput;

BufferOutput* buffer_init(int size);
void buffer_free(BufferOutput* context);
void buffer_clear(BufferOutput* context);
char* buffer_get(BufferOutput* context);
size_t buffer_size(BufferOutput* context);
void buffer_append_line(BufferOutput* context, const char* buff);
void buffer_append_linef(BufferOutput* context, char* bufftmp, const char *format, ...);

void buffer_append_str(BufferOutput* context, const char* buff);
void buffer_append(BufferOutput* context, const char* buff, int size);

void test_malloc();
#endif /* TERMINAL_BUFFER_H_ */
