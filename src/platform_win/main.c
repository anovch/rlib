/*
 * main.c
 *
 *  Created on: 15 gru 2018
 *      Author: lukasz
 */
#include <stdio.h>
#include <test/tests.h>
#include <core/startup.h>
#include "platform/logger.h"
#include <stdarg.h>

void logger(LOGGER_LEVEL level, const char *format, ...){
	va_list args;
	va_start(args, format);
	if (level == LL_INFO) {
		printf("INFO:");
	}
	if (level == LL_ERROR) {
		printf("ERROR:");
	}
	if (level == LL_DEBUG) {
		printf("DEBUG:");
	}

	vprintf(format, args);
	printf("\n");

	va_end(args);
	fflush(stdout);

}

int main(int argc, char** argv) {
//	printf("Start tests\n");
//	full_test_terminal();
//	test_params();
//	serial_test();

	startup();

	return 1;
}

