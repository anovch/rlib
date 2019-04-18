/*
 * commands.c
 *
 *  Created on: 19 gru 2018
 *      Author: lukasz
 */
#include "terminal/buffer.h"
#include "terminal/line_splitter.h"
#include "terminal/line_executor.h"
#include "platform/pwm.h"
#include "core/startup.h"
#include "core/sys_params.h"
#include <stdio.h>
#include <string.h>
#include "platform/mem.h"

char* help[] = {
	"---------Help--------------",
	"help                               - help",
	"params                             - system parameters",
	"wifi [run/stop/info]               - run/stop/state wifi",
	"set [ap/pass/port/wifion/wifioff]  - set parameters",
	"pwm channel val                    - set pwm ",
	"log 0/1                            - log on off",
	"dir channel val                    - set direction",
	"run val1 dir1 val2 dir2            - run",
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
	if (value == NULL) {
		sprintf(buff,"%s : --- ",name);
	}
	else {
		sprintf(buff,"%s : %s",name, value);
	}
	return buff;
}
static char output[128];

static unsigned char callback_params(int argc, char** argv, BufferOutput* result) {
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

static unsigned char callback_wpm(int argc, char** argv, BufferOutput* result) {
	if (argc == 2) {
		unsigned int channel = atoi(argv[0]); // channel
		unsigned int value = atoi(argv[1]); // value
		set_pwm(channel,value);
	}
	buffer_append_line(result, "END.");
	return 1;

}

static unsigned char callback_log(int argc, char** argv, BufferOutput* result) {
	if (argc == 1) {
		unsigned int on = atoi(argv[0]); //
		set_logger(on);
	}
	buffer_append_line(result, "END.");
	return 1;

}

static unsigned char callback_dir(int argc, char** argv, BufferOutput* result) {
	if (argc == 2) {
		unsigned int channel = atoi(argv[0]); // channel
		unsigned int value = atoi(argv[1]); // value
		set_dir(channel,value);

	}
	buffer_append_line(result, "END.");
	return 1;

}

static unsigned char callback_run(int argc, char** argv, BufferOutput* result) {
	if (argc == 4) {
		unsigned int val1 = atoi(argv[0]); // val1
		unsigned int dir1 = atoi(argv[1]); // dir1
		unsigned int val2 = atoi(argv[2]); // val2
		unsigned int dir2 = atoi(argv[3]); // dir2
		set_pwm(0,val1);
		set_dir(0,dir1);
		set_pwm(1,val2);
		set_dir(1,dir2);

	}
	buffer_append_line(result, "END.");
	return 1;

}

void commands_register(LineParserContext* executor) {
	line_exec_register(executor,"help",callback_help);
	line_exec_register(executor,"params",callback_params);
	line_exec_register(executor,"wifi",callback_wifi);
	line_exec_register(executor,"set",callback_set);
	line_exec_register(executor,"pwm",callback_wpm);
	line_exec_register(executor,"log",callback_log);
	line_exec_register(executor,"dir",callback_dir);
	line_exec_register(executor,"run",callback_run);

}
