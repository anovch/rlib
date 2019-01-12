/*
 * terminal_interface.h
 *
 *  Created on: 15 gru 2018
 *      Author: lukasz
 */

#ifndef TERMINAL_TERMINAL_H_
#define TERMINAL_TERMINAL_H_

#include "terminal/buffer.h"
#include "terminal/line_executor.h"

typedef struct _TerminalContext TerminalContext;
TerminalContext* terminal_init(LineParserContext* executor);
void terminal_free(TerminalContext* context);
void terminal_set_echo(TerminalContext* context, unsigned char echo);
void terminal_get_header(TerminalContext* context, BufferOutput* buffoutput);

void terminal_input(TerminalContext* context, const char* buffin, int size, BufferOutput* buffoutput);

#endif /* TERMINAL_TERMINAL_H_ */
