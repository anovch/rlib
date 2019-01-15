/*
 * serial.h
 *
 *  Created on: 15 gru 2018
 *      Author: lukasz
 */

#ifndef SERIAL_H_
#define SERIAL_H_

#include <terminal/terminal.h>
#include <utils/process_io.h>

typedef struct _SerialCB SerialCB;
struct _SerialCB{
	void* hndl;
	TerminalContext* term;
	ProcessIo* io;
	unsigned char emul;
	void* usr;
};


#define USART1_ 0
#define USART2_ 1
#define USART3_ 2
#define UART_AMOUNT 3

void init_uart();
SerialCB* open_serial(int usart, int baud_rate);
void close_serial(SerialCB* cb);
unsigned char write_serial(SerialCB* cb, const char* buff, int size);
void start_read_serial_term(SerialCB* cb, TerminalContext* term);
void start_read_serial_io(SerialCB* cb, ProcessIo* io);



#endif /* SERIAL_H_ */
