/*
 * ssocket.h
 *
 *  Created on: 5 sty 2019
 *      Author: tolek
 */

#ifndef PLATFORM_WIN_SSOCKET_H_
#define PLATFORM_WIN_SSOCKET_H_
#include "platform/serial.h"
#include "terminal/buffer.h"

SerialCB* open_ssocket(int usart, int baud_rate);
void close_ssocket(SerialCB* cb);
unsigned char write_ssocket(SerialCB* cb, const char* buff, int size);
void start_read_ssocket(SerialCB* cb, TerminalContext* term);


#endif /* PLATFORM_WIN_SSOCKET_H_ */
