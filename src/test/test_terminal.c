/*
 * test_terminal.c
 *
 *  Created on: 15 gru 2018
 *      Author: lukasz
 */
#include <string.h>
#include <stdio.h>
#include <terminal/line_splitter.h>
#include <terminal/line_executor.h>


static unsigned char compare(const char* input1, int  input1_size, const char* input2, int  input2_size) {
	if (input1_size!=input2_size) {
		return 0;
	}
	for (int i=0;i<input1_size;i++) {
		if (input1[i] != input2[i]) {
			return 0;
		}
	}
	return 1;
}

// wprowadzenie linii
const char* input1 = "1234567890\n";
static void test1(InputSplitterContext* term) {
	for (unsigned int i=0;i<strlen(input1);i++) {
		if (line_splitter_input(term,input1[i])) {
			if (compare(input1, strlen(input1)-1, line_splitter_get_buffer(term), line_splitter_get_buffer_size(term))) {
				printf("Terminal Test1 PASS\n");
			}
			else {
				printf("Terminal Test1 ERROR\n");
			}
		}
	}
}

// wprowadzenie dwoch linii
const char* input2_1 = "1234567890\n\r\n1234";
const char* input2_2 = "567890\r123";
const char* input2_1t = "1234567890";
const char* input2_2t = "1234567890";

static void test2(InputSplitterContext* term) {
	line_splitter_clear(term);
	unsigned char pass = 1;
	for (unsigned int i=0;i<strlen(input2_1);i++) {
		if (line_splitter_input(term,input2_1[i])) {
			if (line_splitter_get_buffer_size(term)!=0) {
				if (!compare(input2_1t, strlen(input2_1t), line_splitter_get_buffer(term), line_splitter_get_buffer_size(term))) {
					pass = 0;
				}
			}
			line_splitter_clear(term);
		}
	}
	for (unsigned int i=0;i<strlen(input2_2);i++) {
		if (line_splitter_input(term,input2_2[i])) {
			if (line_splitter_get_buffer_size(term)!=0) {
				if (!compare(input2_2t, strlen(input2_2t), line_splitter_get_buffer(term), line_splitter_get_buffer_size(term))) {
					pass = 0;
				}
			}
		}
	}
	if (pass) {
		printf("Terminal Test2 PASS\n");
	}
	else {
		printf("Terminal Test2 ERROR\n");
	}


}

static unsigned char callback(int argc, char** argv, BufferOutput* result) {
	buffer_append_str(result, "callback ");
	for (int i=0;i<argc;i++) {
		buffer_append_str(result, " ");
		buffer_append_str(result, argv[i]);
	}
	return 1;
}
const char* test3line_in = "   command1 1 2 \t 3    asdasdad ";
const char* test3line_out = "callback  1 2 3 asdasdad";
static void test3() {
	BufferOutput* outbuff = buffer_init(256);

	LineParserContext* exec = line_exec_init();
	line_exec_register(exec,"command1",callback);

	int resutl = 1;
	resutl &= line_exec_run(exec, "comm", outbuff); // skrot
	resutl &= !line_exec_run(exec, "comm3", outbuff); // brak takiego razkazu
	buffer_clear(outbuff);
	resutl &= line_exec_run(exec, test3line_in, outbuff); // rozkaz z parametrami
	if (!compare(buffer_get(outbuff), strlen(buffer_get(outbuff)), test3line_out, strlen(test3line_out))) {
		resutl = 0;
	}

	if (resutl) {
		printf("Terminal Test3 PASS\n");
	}
	else {
		printf("Terminal Test3 ERROR\n");
	}


	line_exec_free(exec);
	buffer_free(outbuff);
}


void full_test_terminal() {
	InputSplitterContext* term = line_splitter_init();
	test1(term);
	test2(term);
	test3();

	line_splitter_free(term);
}
