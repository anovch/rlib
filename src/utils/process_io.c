/*
 * process_core.c
 *
 *  Created on: 17 gru 2018
 *      Author: lukasz
 */
#include <stdlib.h>
#include <string.h>
#include <utils/process_io.h>
#include "platform/mem.h"


ProcessIo* process_io_init(lp_step step, int sleep_ms, lp_read read, lp_write write) {
	ProcessIo* result;
	result = (ProcessIo*)port_malloc(sizeof(ProcessIo));
	memset((void*)result,0,sizeof(ProcessIo));
	result->step = step;
	result->read = read;
	result->write = write;
	result->sleep_ms = sleep_ms;
	return result;

}

void process_io_free(ProcessIo* process) {
	port_free(process);
}
