/*
 * terminal.h
 *
 *  Created on: 15 gru 2018
 *      Author: lukasz
 */

#ifndef LINE_SPLITTER_H_
#define LINE_SPLITTER_H_

typedef struct _InputSplitterContext InputSplitterContext;

InputSplitterContext* line_splitter_init();
InputSplitterContext* line_splitter_init_size(int size);
void line_splitter_free(InputSplitterContext* context);
void line_splitter_set_separators(InputSplitterContext* context, const char* sep);
unsigned char line_splitter_input(InputSplitterContext* context, char byte);
char* line_splitter_get_buffer(InputSplitterContext* context);
int line_splitter_get_buffer_size(InputSplitterContext* context);
void line_splitter_clear(InputSplitterContext* context);

#endif /* LINE_SPLITTER_H_ */
