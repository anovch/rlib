/*
 * buffer.c
 *
 *  Created on: 15 gru 2018
 *      Author: lukasz
 */
#include "terminal/buffer.h"
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

struct _BufferOutput{
	char* buff;
	size_t size;
	size_t capacity;
	unsigned char overload;
	char nl[3];
};



BufferOutput* buffer_init(int size) {
	BufferOutput* result;
	result = (BufferOutput*)malloc(sizeof(BufferOutput));
	result->size = 0;
	result->capacity = size;
	result->overload = 0;
	result->nl[0] = '\r';
	result->nl[1] = '\n';
	result->nl[2] = 0;
	result->buff = (char*)malloc(result->capacity);
	buffer_clear(result);
	return result;

}

void buffer_clear(BufferOutput* context) {
	context->buff[0] = 0;
	context->size = 0;
}

char* buffer_get(BufferOutput* context) {
	return context->buff;
}

size_t buffer_size(BufferOutput* context) {
	return context->size;
}



void buffer_append_str(BufferOutput* context, const char* buff) {
	size_t newsize = context->size + strlen(buff);
	if ((context->size + strlen(buff)) >= context->capacity) {
		context->overload = 1;
		return;
	}
	strcat(context->buff, buff);
	context->buff[newsize] = 0;
	context->size = strlen(context->buff);
}

void buffer_append_line(BufferOutput* context, const char* buff) {
	buffer_append_str(context,buff);
	buffer_append_str(context,context->nl);
}

void buffer_append_linef(BufferOutput* context, char* bufftmp, const char *format, ...) {
	va_list args;
	va_start(args, format);
	vsprintf(bufftmp, format, args);
	buffer_append_line(context, bufftmp);
	va_end(args);
}



void buffer_append(BufferOutput* context, const char* buff, int size) {
	if ((context->size + size) >= context->capacity) {
		context->overload = 1;
		return;
	}
	memcpy(context->buff+context->size, buff, size);
	context->size = context->size+size;
	context->buff[context->size] = 0;
}


void buffer_free(BufferOutput* context) {
	free(context->buff);
	free(context);
}

void test_malloc() {
	char* test = malloc(10);
	test[0] = 0;
	free(test);
}


