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

#endif /* CORE_STARTUP_H_ */
