/*
 * commands.c
 *
 *  Created on: 19 gru 2018
 *      Author: lukasz
 */
#include "terminal/buffer.h"
#include "terminal/line_splitter.h"
#include "terminal/line_executor.h"
#include "core/startup.h"
#include "core/sys_params.h"
#include <stdio.h>
#include <string.h>

char* help[] = {
	"---------Help--------------",
	"help					- pomoc",
	"params					- parametry systemowe",
	"wifi [run/stop/info]			- run/stop/status wifi",
	"set [ap/pass/port/wifion/wifioff]	- ustawienie parametrow",
	"help3 - pomoc",
	"help4 - pomoc",
	"END.",
	0
};

static unsigned char callback_help(int argc, char** argv, BufferOutput* result) {
	int i=0;
	while(1) {
		if (!help[i]) {
			break;
		}
		buffer_append_line(result, help[i]);
		i++;
	}
	return 1;
}

static const char* get_par(char* buff, char* name, char* value) {
	sprintf(buff,"%s : %s",name, value);
	return buff;
}
static unsigned char callback_params(int argc, char** argv, BufferOutput* result) {
	char output[128];
	buffer_append_line(result, "---------Parameters--------------");
	buffer_append_line(result, get_par(output,"AP",sys_params_get("ap")));
	buffer_append_line(result, get_par(output,"PASS",sys_params_get("pass")));
	buffer_append_line(result, get_par(output,"TCP PORT",sys_params_get("tcpport")));
	buffer_append_line(result, get_par(output,"WIFI ON",sys_params_get("wifi_on")));
	buffer_append_line(result, "END.");
	return 1;
}

static unsigned char callback_wifi(int argc, char** argv, BufferOutput* result) {
	if (argc == 1) {
		if (argv[0][0] == 'r') {
			buffer_append_line(result, "run wifi");
			esp8266_start(get_wifi());
		}
		if (argv[0][0] == 's') {
			buffer_append_line(result, "stop wifi");
			esp8266_stop(get_wifi());
		}
		if (argv[0][0] == 'i') {
			buffer_append_line(result, "---------Wifi status--------------");
			esp8266_get_status(get_wifi(), result);
		}

	}
	buffer_append_line(result, "END.");
	return 1;
}


static unsigned char callback_set(int argc, char** argv, BufferOutput* result) {
	if (argc == 1) {
		if(strcmp(argv[0],"wifion")==0) {
			sys_params_set("wifi_on","1");
		}
		if(strcmp(argv[0],"wifioff")==0) {
			sys_params_set("wifi_on","0");
		}
	}
	if (argc == 2) {
		if(strcmp(argv[0],"ap")==0) {
			sys_params_set("ap",argv[1]);
		}
		if(strcmp(argv[0],"pass")==0) {
			sys_params_set("pass",argv[1]);
		}
		if(strcmp(argv[0],"port")==0) {
			sys_params_set("tcpport",argv[1]);
		}

	}
	sys_store_params();
	buffer_append_line(result, "END.");
	return 1;
}

void commands_register(LineParserContext* executor) {
	line_exec_register(executor,"help",callback_help);
	line_exec_register(executor,"params",callback_params);
	line_exec_register(executor,"wifi",callback_wifi);
	line_exec_register(executor,"set",callback_set);

}
