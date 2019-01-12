/*
 * thread.h
 *
 *  Created on: 17 gru 2018
 *      Author: lukasz
 */

#ifndef PLATFORM_THREAD_H_
#define PLATFORM_THREAD_H_
#include <utils/process_io.h>

int start_thread(ProcessIo* process);
void inf_loop();

#endif /* PLATFORM_THREAD_H_ */
