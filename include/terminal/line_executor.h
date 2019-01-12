/*
 * line_parser.h
 *
 *  Created on: 15 gru 2018
 *      Author: lukasz
 */

#ifndef TERMINAL_LINE_EXECUTOR_H_
#define TERMINAL_LINE_EXECUTOR_H_
#include "terminal/buffer.h"

typedef struct _LineParserContext LineParserContext;

typedef unsigned char(*lp_callback)(int argc, char** argv, BufferOutput* buffoutput);

LineParserContext* line_exec_init();
void line_exec_free(LineParserContext* context);
void line_exec_register(LineParserContext* context, const char* name, lp_callback callback);
unsigned char line_exec_run(LineParserContext* context, const char* line, BufferOutput* buffoutput);


#endif /* TERMINAL_LINE_EXECUTOR_H_ */
