/*
 * terminal.c
 *
 *  Created on: 15 gru 2018
 *      Author: lukasz
 */
#include <stdlib.h>
#include <string.h>
#include <terminal/line_splitter.h>

#define TERMINAL_BUFFER_CAPACITY 128
#define TERMINAL_BUFFER_MAX_SEP 8

struct _InputSplitterContext{
	int id;
	char* input_buffer;
	int input_buffer_size;
	int input_buffer_capacite;
	char separators[TERMINAL_BUFFER_MAX_SEP];
	// przepelnienie
	unsigned char f_overload;
	char lastchar;
};


static int id_terminal=0;
InputSplitterContext* line_splitter_init_size(int size) {
	InputSplitterContext* result;
	result = (InputSplitterContext*)malloc(sizeof(InputSplitterContext));
	memset((void*)result,0,sizeof(InputSplitterContext));

	result->id = id_terminal++;
	result->input_buffer_size = 0;
	result->input_buffer_capacite = size;
	result->f_overload=0;
	result->lastchar=0;
	line_splitter_set_separators(result, "\n\r");
	result->input_buffer = (char*)malloc(result->input_buffer_capacite+2);
	return result;
}

InputSplitterContext* line_splitter_init() {
	return line_splitter_init_size(TERMINAL_BUFFER_CAPACITY);
}

void line_splitter_set_separators(InputSplitterContext* context, const char* sep) {
	int size = strlen(sep);
	memset((void*)context->separators,0,TERMINAL_BUFFER_MAX_SEP);
	for (int i=0;i <size; i++) {
		if (i>=(TERMINAL_BUFFER_MAX_SEP-1)) {
			return;
		}
		context->separators[i] = sep[i];
	}
}

void line_splitter_free(InputSplitterContext* context) {
	free(context->input_buffer);
	free(context);
}

void line_splitter_clear(InputSplitterContext* context) {
	context->input_buffer_size = 0;
	context->input_buffer[0] = 0;
}

char* line_splitter_get_buffer(InputSplitterContext* context) {
	return context->input_buffer;
}
int line_splitter_get_buffer_size(InputSplitterContext* context) {
	return context->input_buffer_size;
}

unsigned char line_splitter_input(InputSplitterContext* context, char byte) {
	int size = strlen(context->separators);
	for (int i=0;i <size; i++) {
		if (byte == context->separators[i]) {
			return 1;
		}
	}


	if (context->input_buffer_size>=context->input_buffer_capacite) {
		context->f_overload=1;
		return 1;
	}
	context->input_buffer[context->input_buffer_size] = byte;
	context->input_buffer[context->input_buffer_size+1] = 0;
	context->input_buffer_size++;
	context->lastchar = byte;
	return 0;
}


