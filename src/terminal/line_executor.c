/*
 * line_parser.c
 *
 *  Created on: 15 gru 2018
 *      Author: lukasz
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <terminal/line_executor.h>
#include <utils/utils.h>
#include "platform/mem.h"

#define LINE_PARSER_NAME_SIZE 64
#define LINE_PARSER_CAP 16


typedef struct {
	char name[LINE_PARSER_NAME_SIZE];
	lp_callback callback;
	unsigned char active;
} LineParserCommand;

struct _LineParserContext{
	LineParserCommand commands[LINE_PARSER_CAP];
};


static unsigned char callback_info(int argc, char** argv, BufferOutput* result) {
	buffer_append_line(result, "------------------------");
	buffer_append_line(result, "rlib ver. 1.0");
	buffer_append_line(result, "------------------------");
	return 1;
}


LineParserContext* line_exec_init() {
	LineParserContext* result;
	result = (LineParserContext*)port_malloc(sizeof(LineParserContext));
	memset((void*)result,0,sizeof(LineParserContext));
	line_exec_register(result,"info",callback_info);

	return result;
}

void line_exec_free(LineParserContext* context) {
	port_free(context);
}


void line_exec_register(LineParserContext* context, const char* name, lp_callback callback) {
	for(unsigned int i=0;i<LINE_PARSER_CAP;i++) {
		if (!context->commands[i].active) {
			strcpy(context->commands[i].name, name);
			context->commands[i].active = 1;
			context->commands[i].callback = callback;
			break;
		}
	}
}


static unsigned char check_str(const char* str1, const char* str2) {
	if ((str1==NULL) || (str2 == NULL)) {
		return 1;
	}
	while(*str1) {
		if ((*str1) != (*str2)) {
			return 0;
		}
		str1++;
		str2++;
	}
	return 1;
}



unsigned char line_exec_run(LineParserContext* context, const char* line, BufferOutput* buffoutput) {

	unsigned char result = 0;

	Splitter* splt = splitter_init(line, " \t");
	int index = splitter_get_amount_items(splt);

	for(unsigned int i=0;i<LINE_PARSER_CAP;i++) {
		if (context->commands[i].active) {
			if (check_str(splitter_get_item(splt, 0), context->commands[i].name)) {
				if (context->commands[i].callback!=NULL) {
					char** it = splitter_get_item_buffer(splt);
					(*context->commands[i].callback)(index-1, &it[1], buffoutput);
				}
				result = 1;
			}
		}
	}

	splitter_free(splt);
	return result;
}


