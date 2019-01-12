/*
 * process_core.h
 *
 *  Created on: 17 gru 2018
 *      Author: lukasz
 */

#ifndef UTILS_PROCESS_IO_H_
#define UTILS_PROCESS_IO_H_
#include <stddef.h>

typedef struct _ProcessIo ProcessIo;

typedef void(*lp_step)(ProcessIo* io);
typedef unsigned char (*lp_read)(ProcessIo* io, const char* buff, size_t len);
typedef unsigned char (*lp_write)(ProcessIo* io, const char* buff, size_t len);


struct _ProcessIo{
	lp_step step;
	lp_read read;
	lp_write write;
	int sleep_ms;
	void* user_object;
};




ProcessIo* process_io_init(lp_step step, int sleep_ms, lp_read read, lp_write write);
void process_io_free(ProcessIo* process);




#endif /* UTILS_PROCESS_IO_H_ */
