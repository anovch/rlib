/*
 * thread.c
 *
 *  Created on: 17 gru 2018
 *      Author: lukasz
 */
#include <windows.h>
#include "utils/process_io.h"
#include "platform/mem.h"


DWORD WINAPI ThreadFunc(void* data) {
  ProcessIo* process = (ProcessIo*)data;
  while(1) {
	 if (process->step!=NULL) {
		 (*process->step)(process);
	 }
	 Sleep(process->sleep_ms);
  }

  return 0;
}


int start_thread(ProcessIo* process) {
  HANDLE thread = CreateThread(NULL, 0, ThreadFunc, process, 0, NULL);
  if (thread) {
    // Optionally do stuff, such as wait on the thread.
  }
  return 0;
}



void inf_loop() {
  while(1) {
		 Sleep(1000);
  }

}


void *port_malloc( size_t xWantedSize ) {
	return malloc(xWantedSize);
}

void port_free(void *pv ) {
	free(pv);
}


void set_pwm(int channel, unsigned int value) {

}

