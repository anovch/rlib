/*
 * serial_test.c
 *
 *  Created on: 15 gru 2018
 *      Author: lukasz
 */
#include "platform/serial.h"
#include "platform/thread.h"
#include <stdio.h>
#include <string.h>
#include <terminal/terminal.h>
#include <esp8266/esp8266.h>
#include <utils/process_io.h>

const char* str = "Test\n";
static unsigned char write(ProcessIo* io, const char* buff, size_t len) {
	//printf("----->%s", buff);
	(*io->read)(io, str, strlen(str));
	return 0;
}

void serial_test() {


	Esp8266Context* c = esp8266_init(0);
	esp8266_register_write(c, write);
	ProcessIo* io = esp8266_get_io(c);
	start_thread(io);
	(*io->read)(io, str, strlen(str));


//	ProcessIo* prc = process_io_init(step, 1000, NULL, NULL);
//	start_thread(prc);

	SerialCB* port = open_serial(USART1,9600);
	fflush(stdout);

//	const char* string = "String testowy\n";
//	write_serial(port,string,strlen(string));
//	fflush(stdout);
//	read_serial(port);
//	close_serial(port);

	LineParserContext* executor = line_exec_init();
	TerminalContext* term = terminal_init(executor);
	start_read_serial_term(port, term);



}
