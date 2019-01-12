/*
 * process_core.c
 *
 *  Created on: 17 gru 2018
 *      Author: lukasz
 */
#include <stdlib.h>
#include <string.h>
#include <utils/process_io.h>


ProcessIo* process_io_init(lp_step step, int sleep_ms, lp_read read, lp_write write) {
	ProcessIo* result;
	result = (ProcessIo*)malloc(sizeof(ProcessIo));
	memset((void*)result,0,sizeof(ProcessIo));
	result->step = step;
	result->read = read;
	result->write = write;
	result->sleep_ms = sleep_ms;
	return result;

}

void process_io_free(ProcessIo* process) {
	free(process);
}
