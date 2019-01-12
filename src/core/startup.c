/*
 * startup.c
 *
 *  Created on: 19 gru 2018
 *      Author: lukasz
 */
#include "core/startup.h"
#include "core/sys_params.h"
#include "platform/serial.h"
#include "platform/thread.h"
#include "terminal/line_executor.h"
#include "esp8266/esp8266.h"
#include "utils/process_io.h"

static TerminalContext* term;
static LineParserContext* executor;
static SerialCB* usart1; // terminal
static SerialCB* usart2; // esp8266
static Esp8266Context* esp8266;

void commands_register(LineParserContext* executor);

Esp8266Context*  get_wifi() {
	return esp8266;
}



static unsigned char esp8266_write(ProcessIo* io, const char* buff, size_t len) {
	write_serial(usart2, buff, len);
	return 0;
}

void startup() {

	sys_init_params();

	sys_params_set_default("ap","UPC542C3E9");
	sys_params_set_default("pass","B4njeawec3md");
	sys_params_set_default("tcpport","3333");
	sys_params_set_default("wifi_on","1");

	// terminal na usart1
	executor = line_exec_init();
	commands_register(executor);

	// rs terminlalowy
	term = terminal_init(executor);
	usart1 = open_serial(USART1,9600);
	start_read_serial_term(usart1, term);


	// esp8266
	esp8266 = esp8266_init(executor);
	esp8266_register_write(esp8266, esp8266_write);
	ProcessIo* io = esp8266_get_io(esp8266);
	usart2 = open_serial(USART2,115200);
	start_read_serial_io(usart2, io);
	start_thread(io);




	inf_loop();
}

