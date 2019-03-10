/*
 * terminal_interface.c
 *
 *  Created on: 15 gru 2018
 *      Author: lukasz
 */
#include <stdlib.h>
#include <string.h>
#include <terminal/terminal.h>
#include "terminal/line_splitter.h"
#include "platform/mem.h"


struct _TerminalContext{
	InputSplitterContext* splitter;
	LineParserContext* executor;
	unsigned char echo;
};



TerminalContext* terminal_init(LineParserContext* executor) {
	TerminalContext* result;
	result = (TerminalContext*)port_malloc(sizeof(TerminalContext));
	result->splitter = line_splitter_init();
	result->executor = executor;
	result->echo = 1;
	return result;
}

void terminal_set_echo(TerminalContext* context, unsigned char echo) {
	context->echo = echo;
}


void terminal_free(TerminalContext* context)  {
	line_splitter_free(context->splitter);
	port_free(context);
}


const char* term_header = "Terminal interface ver 1.0";
const char* term_error = "Command not found";
void terminal_get_header(TerminalContext* context, BufferOutput* buffoutput)  {
	buffer_append_line(buffoutput,term_header);
}

void terminal_input(TerminalContext* context, const char* buffin, int size, BufferOutput* buffoutput)  {
	buffer_clear(buffoutput);

	// localecho
	if (context->echo) {
		buffer_append(buffoutput, buffin, size);
	}

	for (int i=0; i<size;i++) {

		if (line_splitter_input(context->splitter,buffin[i])) {
			if (line_splitter_get_buffer_size(context->splitter) == 0) {
				// nacisnietyu enter - bufir pusty
				continue;
			}

			if (!line_exec_run(context->executor, line_splitter_get_buffer(context->splitter),buffoutput)) {
				buffer_append_line(buffoutput,term_error);
			}
			line_splitter_clear(context->splitter);
		}
	}
}



