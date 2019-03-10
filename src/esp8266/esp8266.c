/*
 * esp8266.c
 *
 *  Created on: 17 gru 2018
 *      Author: lukasz
 */
#include "esp8266/esp8266.h"
#include "esp8266/esp8266_linesplitter.h"
#include "utils/process_io.h"
#include "utils/utils.h"
#include "platform/logger.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <terminal/line_splitter.h>
#include <terminal/terminal.h>
#include "platform/mem.h"


typedef enum {
	EPS8266_INIT,
	EPS8266_INIT_WIFI,
	EPS8266_RELOAD,
	EPS8266_STOP,
} EPS8266MODE;

typedef enum {
	AT_NONE, AT_AT, AT_CWMODE, AT_CWJAP_CUR, AT_CIPMUX, AT_CIPSERVER, AT_CIFSR, AT_CIPSEND, AT_CWQAP

} EPS8266AT;

#define COM_MAX_LEN 256
static unsigned char prefix(const char *pre, const char *str) {
	return strncmp(pre, str, strlen(pre)) == 0;
}

#define MAXWIFISESS 2
typedef struct {
	unsigned char connected;
	TerminalContext* term;
	BufferOutput* buffout;
	int rec_bytes;
	int send_bytes;
} Esp8266Session;

#define STATUS_LEN 64
struct _Esp8266Context {
	ProcessIo* io;
	InputEsp8266SplitterContext* input;
	EPS8266MODE mode;
	char last_com[COM_MAX_LEN];
	EPS8266AT last_at;
	Esp8266Session session[MAXWIFISESS];

	// status
	char ip[STATUS_LEN];
	char mac[STATUS_LEN];
	unsigned char wifi_connected;

	int curr_sess_write;
};

//AT+CWMODE=1
//AT+CWJAP_CUR="UPC542C3E9","B4njeawec3md"
//WIFI DISCONNECT
//WIFI CONNECTED
//WIFI GOT IP

static void run_command(ProcessIo* io, char* com, EPS8266AT at) {
	Esp8266Context* c = (Esp8266Context*) (io->user_object);
	c->last_at = at;
	strcpy(c->last_com, com);
	strcat(c->last_com, "\r\n");
	io->write(io, c->last_com, strlen(c->last_com));
	logger(LL_DEBUG, "wr 8266: '%s'", com);

}


static int get_number_item(Splitter* sp, int index) {
	if (splitter_get_amount_items(sp)>=index) {
		char* c = splitter_get_item(sp,index);
		return atoi(c);
	}
	return 0;
}

static void session_write_data(ProcessIo* io) {
	// jest zacheta do wprowadzenia danych
	Esp8266Context* c = (Esp8266Context*) (io->user_object);
	char* buff = buffer_get(c->session[c->curr_sess_write].buffout);
	c->last_at = AT_NONE;
	io->write(io, buff, strlen(buff));
	logger(LL_DEBUG, "wr_data 8266: '%s'", buff);
}

static void session_write(ProcessIo* io, int sess, int size) {
	//AT+CIPSEND=0,4
	Esp8266Context* c = (Esp8266Context*) (io->user_object);
	char buff[32];
	c->curr_sess_write = sess;
	sprintf(buff, "AT+CIPSEND=%i,%i",sess,size);
	run_command(io, buff, AT_CIPSEND);
}

//	DEBUG:rd 8266: '0,CONNECT'
//	DEBUG:rd 8266: '+IPD,0,1:a'
//	DEBUG:rd 8266: '0,CLOSED'
static void session_read(ProcessIo* io, char* line) {
	Esp8266Context* c = (Esp8266Context*) (io->user_object);
	int sess;
	int size;
	int offset;
	esp8266_line_splitter_get_ipd(c->input, &sess, &size, &offset);


	if (offset == 0) {
		logger(LL_ERROR, "symbol ':' not found");
		return;
	}

	char* data = line+offset;
	terminal_input(c->session[sess].term, data, size, c->session[sess].buffout);
	int outsize = buffer_size(c->session[sess].buffout);
	if (outsize!=0) {
		session_write(io, sess, outsize);
	}

	if ((sess >=0) && (sess<MAXWIFISESS)) {
		logger(LL_INFO, "TCP REC %i: '%s'", sess, data);
	}

}

static void session_connect(ProcessIo* io, char* line) {
	Esp8266Context* c = (Esp8266Context*) (io->user_object);
	Splitter* sp = splitter_init(line, ",");
	int sess = get_number_item(sp,0);
	if ((sess >=0) && (sess<MAXWIFISESS)) {
		c->session[sess].connected = 1;
		logger(LL_INFO, "TCP CONNECTED %i", sess);

		buffer_clear(c->session[sess].buffout);
		terminal_get_header(c->session[sess].term, c->session[sess].buffout);
		session_write(io, sess, strlen(buffer_get(c->session[sess].buffout)));

	}
	splitter_free(sp);
}

static void session_closed(ProcessIo* io, char* line) {
	Esp8266Context* c = (Esp8266Context*) (io->user_object);
	Splitter* sp = splitter_init(line, ",");
	int index = get_number_item(sp,0);
	if ((index >=0) && (index<MAXWIFISESS)) {
		c->session[index].connected = 0;
		logger(LL_INFO, "TCP DISCONNECTED %i", index);
	}
	splitter_free(sp);
}

static void responce(ProcessIo* io, char* line) {
	Esp8266Context* c = (Esp8266Context*) (io->user_object);
	if (prefix(line, c->last_com)) {
		// echo raozkazu
		logger(LL_DEBUG, "wr echo 8266: '%s'", line);
		return;
	}
	logger(LL_DEBUG, "rd 8266: '%s'", line);

	if (prefix("WIFI CONNECTED", line)) {
		logger(LL_INFO, "WIFI CONNECTED");
		c->wifi_connected = 1;
	}
	if (prefix("WIFI DISCONNECT", line)) {
		logger(LL_INFO, "WIFI DISCONNECT");
		c->wifi_connected = 0;
	}

	unsigned char ok = 0;
	if (prefix(line, "OK")) {
		ok = 1;
	}

	//	DEBUG:wr 8266: 'AT+CIFSR'
	//	DEBUG:rd 8266: '+CIFSR:STAIP,"192.168.0.109"'
	//	DEBUG:rd 8266: '+CIFSR:STAMAC,"cc:50:e3:55:be:b4"'
	//	DEBUG:rd 8266: 'OK'
	if (prefix("+CIFSR:STAIP", line)) {
		logger(LL_INFO, line);
		Splitter* sp = splitter_init(line, ",");
		strcpy(c->ip,splitter_get_item(sp,1));
		splitter_free(sp);
	}
	if (prefix("+CIFSR:STAMAC", line)) {
		logger(LL_INFO, line);
		Splitter* sp = splitter_init(line, ",");
		strcpy(c->mac,splitter_get_item(sp,1));
		splitter_free(sp);
	}

	if (prefix(",CONNECT", line+1)) {
		session_connect(io,line);
	}
	if (prefix(",CLOSED", line+1)) {
		session_closed(io,line);
	}
	if (prefix("+IPD,", line)) {
		session_read(io,line);
	}


	if (ok) {
		switch (c->last_at) {
		case AT_AT:
			run_command(io, "AT+CWMODE=1", AT_CWMODE);
			c->mode = EPS8266_INIT_WIFI;
			break;
		case AT_CWMODE:
			run_command(io, "AT+CWJAP_CUR=\"UPC542C3E9\",\"B4njeawec3md\"",
					AT_CWJAP_CUR);
			break;
		case AT_CWJAP_CUR:
			run_command(io, "AT+CIPMUX=1", AT_CIPMUX);
			break;
		case AT_CIPMUX:
			run_command(io, "AT+CIPSERVER=1,3333", AT_CIPSERVER);
			break;
		case AT_CIPSERVER:
			run_command(io, "AT+CIFSR", AT_CIFSR);
			c->mode = EPS8266_INIT_WIFI;
			break;
		default:
			break;
		}
	}

	switch (c->mode) {
	case EPS8266_INIT:
		c->wifi_connected = 0;
		break;
	default:
		break;
	}

}

static void step(ProcessIo* io) {
	Esp8266Context* c = (Esp8266Context*) (io->user_object);

	switch (c->mode) {
	case EPS8266_INIT:
		run_command(io, "at", AT_AT);
		break;
	case EPS8266_RELOAD:
		run_command(io, "AT+CWQAP", AT_CWQAP);
		c->mode = EPS8266_INIT;
		break;
	default:
		break;
	}
}

static unsigned char read(ProcessIo* io, const char* buff, size_t len) {
	Esp8266Context* c = (Esp8266Context*) (io->user_object);
	for (size_t i = 0; i < len; i++) {
		if (esp8266_line_splitter_input(c->input, buff[i])) {
			char* resp = esp8266_line_splitter_get_buffer(c->input);
			if (strlen(resp) != 0) {
				responce(io, resp);
			}
			esp8266_line_splitter_clear(c->input);
		}
		if (c->last_at == AT_CIPSEND) {
			char* resp = esp8266_line_splitter_get_buffer(c->input);
			if (resp[0] == '>') {
				esp8266_line_splitter_clear(c->input);
				session_write_data(io);
			}
		}
	}

	return 0;
}

void esp8266_start(Esp8266Context* context) {
	context->mode = EPS8266_RELOAD;
}

void esp8266_stop(Esp8266Context* context) {
	context->mode = EPS8266_STOP;
}


void esp8266_get_status(Esp8266Context* context, BufferOutput* result) {
	char buff[64];
	buffer_append_linef(result, buff,"ip : %s ", context->ip);
	buffer_append_linef(result, buff,"mac : %s ", context->mac);
	buffer_append_linef(result, buff,"wifi conn : %i", context->wifi_connected);
}


Esp8266Context* esp8266_init(LineParserContext* executor) {
	Esp8266Context* result;
	result = (Esp8266Context*) port_malloc(sizeof(Esp8266Context));
	memset((void*) result, 0, sizeof(Esp8266Context));
	result->io = process_io_init(step, 1000, read, NULL);
	result->io->user_object = result;
	result->input = esp8266_line_splitter_init(512);
	result->mode = EPS8266_INIT;
	memset(result->ip,0,STATUS_LEN);
	memset(result->mac,0,STATUS_LEN);

	for (int i=0;i<MAXWIFISESS;i++) {
		result->session[i].connected = 0;
		result->session[i].term = terminal_init(executor);
		terminal_set_echo(result->session[i].term,0);
		result->session[i].buffout = buffer_init(512);
	}
	return result;

}

ProcessIo* esp8266_get_io(Esp8266Context* context) {
	return context->io;
}

void esp8266_register_write(Esp8266Context* context, lp_write write) {
	context->io->write = write;
}

void esp8266_free(Esp8266Context* context) {
	process_io_free(context->io);
	port_free(context);
}

