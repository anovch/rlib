/*
 * esp8266.h
 *
 *  Created on: 17 gru 2018
 *      Author: lukasz
 */

#ifndef ESP8266_H_
#define ESP8266_H_

#include <utils/process_io.h>
#include "terminal/line_executor.h"
#include "terminal/buffer.h"


typedef struct _Esp8266Context Esp8266Context;

Esp8266Context* esp8266_init(LineParserContext* executor);
void esp8266_free(Esp8266Context* context);
void esp8266_register_write(Esp8266Context* context, lp_write write);
ProcessIo* esp8266_get_io(Esp8266Context* context);

void esp8266_start(Esp8266Context* context);
void esp8266_stop(Esp8266Context* context);
void esp8266_get_status(Esp8266Context* context, BufferOutput* result);

#endif /* ESP8266_H_ */
