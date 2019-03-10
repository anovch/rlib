/*
 * startup.h
 *
 *  Created on: 19 gru 2018
 *      Author: lukasz
 */

#ifndef CORE_STARTUP_H_
#define CORE_STARTUP_H_
#include "esp8266/esp8266.h"

void startup();

Esp8266Context*  get_wifi();
unsigned char terminal_write(const char* buff, size_t len);

#endif /* CORE_STARTUP_H_ */
