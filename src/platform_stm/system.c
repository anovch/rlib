/*
 * system.c
 *
 *  Created on: 15.01.2019
 *      Author: tolek
 */

#include <stdio.h>
#include <test/tests.h>
#include <core/startup.h>
#include "platform/logger.h"
#include <stdarg.h>
#include "utils/process_io.h"

void logger(LOGGER_LEVEL level, const char *format, ...){
	va_list args;
	va_start(args, format);
	va_end(args);
}



int start_thread(ProcessIo* process) {
  return 0;
}



void inf_loop() {
}
