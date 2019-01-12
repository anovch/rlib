/*
 * esp8266_linesplitter.c
 *
 *  Created on: 5 sty 2019
 *      Author: tolek
 */
#include "esp8266/esp8266_linesplitter.h"
#include "terminal/line_splitter.h"
#include "utils/utils.h"
#include <stdlib.h>
#include <string.h>

struct _InputEsp8266SplitterContext{
	InputSplitterContext* linesplitter;
	//+IPD
	unsigned char ipd_comm;
	int ipd_size;
	int ipd_sess;
	int ipd_data_offset;
	int ipd_cur_size;

};

InputEsp8266SplitterContext* esp8266_line_splitter_init() {
	return esp8266_line_splitter_init_size(128);
}

InputEsp8266SplitterContext* esp8266_line_splitter_init_size(int size){
	InputEsp8266SplitterContext* result;
	result = (InputEsp8266SplitterContext*)malloc(sizeof(InputEsp8266SplitterContext));
	memset((void*)result,0,sizeof(InputEsp8266SplitterContext));
	result->linesplitter = line_splitter_init_size(size);
	result->ipd_comm = 0;
	result->ipd_size = -1;
	result->ipd_cur_size = 0;
	line_splitter_set_separators(result->linesplitter,"");
	return result;
}

void esp8266_line_splitter_free(InputEsp8266SplitterContext* context){
	line_splitter_free(context->linesplitter);
	free(context);
}

static unsigned char prefix(const char *pre, const char *str) {
	return strncmp(pre, str, strlen(pre)) == 0;
}

static int get_number_item(Splitter* sp, int index) {
	if (splitter_get_amount_items(sp)>=index) {
		char* c = splitter_get_item(sp,index);
		return atoi(c);
	}
	return 0;
}

void esp8266_line_splitter_get_ipd(InputEsp8266SplitterContext* context, int *sess, int *size, int *offset) {
	*sess = context->ipd_sess;
	*size = context->ipd_size;
	*offset = context->ipd_data_offset;
}
/*
 * Odbieramy bajby
 * zwracany string jezeli koniec linii \n lub \r
 * w przypadku gdy jest rozkaz +IPD (odczytane dane) to analizowana ilosc danych do odczytu
 *
 */
unsigned char esp8266_line_splitter_input(InputEsp8266SplitterContext* context, char byte) {
	char* sb;

	if (context->ipd_comm) {
		line_splitter_input(context->linesplitter, byte);
		sb = line_splitter_get_buffer(context->linesplitter);
		if(context->ipd_size<0) {
			if (byte == ':') {
				Splitter* sp = splitter_init(sb, ",");
				context->ipd_sess = get_number_item(sp,1);
				context->ipd_size = get_number_item(sp,2);
				context->ipd_data_offset = strlen(sb);
				splitter_free(sp);
			}
		}
		else {
			context->ipd_cur_size++;
			if (context->ipd_size == context->ipd_cur_size) {
				return 1;
			}
		}
	}
	else {
		if ((byte == '\n') || (byte == '\r')) {
			return 1;
		}
		line_splitter_input(context->linesplitter, byte);
		sb = line_splitter_get_buffer(context->linesplitter);
		if (sb[0]=='+') {
			if (prefix("+IPD",sb)) {
				context->ipd_comm = 1;
				context->ipd_size = -1;
				context->ipd_cur_size = 0;
			}
		}
	}
	return 0;
}

char* esp8266_line_splitter_get_buffer(InputEsp8266SplitterContext* context) {
	return line_splitter_get_buffer(context->linesplitter);
}

int esp8266_line_splitter_get_buffer_size(InputEsp8266SplitterContext* context){
	return line_splitter_get_buffer_size(context->linesplitter);
}

void esp8266_line_splitter_clear(InputEsp8266SplitterContext* context){
	context->ipd_comm = 0;
	line_splitter_clear(context->linesplitter);
}
