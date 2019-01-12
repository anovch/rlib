/*
 * esp8266_linesplitter.h
 *
 *  Created on: 5 sty 2019
 *      Author: tolek
 */

#ifndef ESP8266_ESP8266_LINESPLITTER_H_
#define ESP8266_ESP8266_LINESPLITTER_H_

typedef struct _InputEsp8266SplitterContext InputEsp8266SplitterContext;

InputEsp8266SplitterContext* esp8266_line_splitter_init();
InputEsp8266SplitterContext* esp8266_line_splitter_init_size(int size);
void esp8266_line_splitter_free(InputEsp8266SplitterContext* context);
unsigned char esp8266_line_splitter_input(InputEsp8266SplitterContext* context, char byte);
void esp8266_line_splitter_get_ipd(InputEsp8266SplitterContext* context, int *sess, int *size, int *offset);
char* esp8266_line_splitter_get_buffer(InputEsp8266SplitterContext* context);
int esp8266_line_splitter_get_buffer_size(InputEsp8266SplitterContext* context);
void esp8266_line_splitter_clear(InputEsp8266SplitterContext* context);

#endif /* ESP8266_ESP8266_LINESPLITTER_H_ */
