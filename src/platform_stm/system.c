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
#include "platform/mem.h"

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"

char buffer[32];
volatile unsigned char logger_active = 0;
void logger(LOGGER_LEVEL level, const char *format, ...){
	if (!logger_active) {
		return;
	}
	memset(buffer,0,32);
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, 32, format, args);
	strcat(buffer,"\r\n");
	terminal_write(buffer,strlen(buffer));
	va_end(args);
}

void logger_(LOGGER_LEVEL level, const char *format, ...){
}

void set_logger(unsigned char on) {
	logger_active = on;
}

static void ProcessTask(void const * argument)
{
  ProcessIo* process = (ProcessIo*)	argument;
  for(;;)
  {
	if (process->step!=NULL) {
	 (*process->step)(process);
	}

	HAL_Delay(process->sleep_ms);
  }
}

int start_thread(ProcessIo* process) {

	osThreadDef(ProcessTask, ProcessTask, osPriorityNormal, 0, 128);
	osThreadCreate(osThread(ProcessTask), process);

	return 0;
}


void vApplicationMallocFailedHook() {
	for(;;){

	}
}


void inf_loop() {
}


void *port_malloc( size_t xWantedSize ) {
	return pvPortMalloc(xWantedSize);
}

void port_free(void *pv ) {
	vPortFree(pv);
}


void mem_alloc_marker(void *pvReturn, size_t xWantedSize) {
	static size_t all = 0;
	all += xWantedSize;

}

void mem_free_marker(void *pv) {

}
