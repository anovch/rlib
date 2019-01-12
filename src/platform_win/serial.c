/*
 * serial.c
 *
 *  Created on: 15 gru 2018
 *      Author: lukasz
 */


#include <stdio.h>
#include "platform/serial.h"
#include "terminal/buffer.h"
#include "windows.h"
#include "winbase.h"
#include "ssocket.h"



SerialCB* open_serial_(int usart, int baud_rate)
{

  const char* portname = NULL;
  if (usart == USART1) {

  }
  if (usart == USART2) {
	  portname = "\\\\.\\COM3";
  }
  if (usart == USART3) {

  }

  SerialCB* result;
  result = (SerialCB*)malloc(sizeof(SerialCB));

  result->emul = 0;
  result->hndl = CreateFile(portname,                //port name "\\\\.\\COM24"
                      GENERIC_READ | GENERIC_WRITE, //Read/Write
                      0,                            // No Sharing
                      NULL,                         // No Security
                      OPEN_EXISTING,// Open existing port only
                      0,            // Non Overlapped I/O
                      NULL);        // Null for Comm Devices

  if (result->hndl == INVALID_HANDLE_VALUE) {
      printf("Error in opening serial port\n");
      return result;
  }


  DCB dcbSerialParams = { 0 }; // Initializing DCB structure
  dcbSerialParams.DCBlength = sizeof(dcbSerialParams);
  if (!GetCommState(result->hndl, &dcbSerialParams)) {
      printf("GetCommState error\n");
      return result;
  }

  dcbSerialParams.BaudRate = baud_rate;  // Setting BaudRate = 9600
  dcbSerialParams.ByteSize = 8;         // Setting ByteSize = 8
  dcbSerialParams.StopBits = ONESTOPBIT;// Setting StopBits = 1
  dcbSerialParams.Parity   = NOPARITY;  // Setting Parity = None

  if (!SetCommState(result->hndl, &dcbSerialParams)) {
	  printf("SetCommState error\n");
	  return result;
  }

  COMMTIMEOUTS timeouts = { 0 };
  timeouts.ReadIntervalTimeout         = 50; // in milliseconds
  timeouts.ReadTotalTimeoutConstant    = 50; // in milliseconds
  timeouts.ReadTotalTimeoutMultiplier  = 1000; // in milliseconds
  timeouts.WriteTotalTimeoutConstant   = 50; // in milliseconds
  timeouts.WriteTotalTimeoutMultiplier = 10; // in milliseconds


  if (!SetCommTimeouts(result->hndl, &timeouts)) {
	  printf("SetCommTimeouts error\n");
  }

  printf("opening serial port successful\n");

  return result;
}

void close_serial_(SerialCB* cb) {
 CloseHandle(cb->hndl);//Closing the Serial Port
}

static void _read_serial(SerialCB* cb, TerminalContext* term) {
	char TempChar; //Temporary character used for reading
	DWORD NoBytesRead;


	BufferOutput* buffout = buffer_init(256);
	terminal_get_header(term, buffout);
	write_serial(cb, buffer_get(buffout),strlen(buffer_get(buffout)));

	while(1)
	{
		ReadFile( cb->hndl,           //Handle of the Serial port
             &TempChar,       //Temporary character
             sizeof(TempChar),//Size of TempChar
             &NoBytesRead,    //Number of bytes read
             NULL);

		if (NoBytesRead>0) {
			//write_serial(hComm, &TempChar,NoBytesRead);
			test_malloc();
			terminal_input(term, &TempChar, NoBytesRead, buffout);
			int size = buffer_size(buffout);
			if (size!=0) {
				write_serial(cb, buffer_get(buffout),size);
			}
		}
	}


}


static void _read_serial_io(SerialCB* cb, ProcessIo* io) {
	char TempChar; //Temporary character used for reading
	DWORD NoBytesRead;

	while(1)
	{
		ReadFile( cb->hndl,           //Handle of the Serial port
             &TempChar,       //Temporary character
             sizeof(TempChar),//Size of TempChar
             &NoBytesRead,    //Number of bytes read
             NULL);

		if (NoBytesRead>0) {
			io->read(io,&TempChar,sizeof(TempChar));
		}
		else {
			usleep(100);
		}
		//printf("read io\n");
		//fflush(stdout);

	}


}

static DWORD WINAPI SerialThreadFunc(void* data) {
  SerialCB* cb = (SerialCB*)data;
  _read_serial(cb,cb->term);
  return 0;
}


void start_read_serial_(SerialCB* cb, TerminalContext* term) {
  cb->term = term;
  HANDLE thread = CreateThread(NULL, 0, SerialThreadFunc, cb, 0, NULL);
  if (thread) {
    // Optionally do stuff, such as wait on the thread.
  }
}

static DWORD WINAPI SerialThreadFuncIo(void* data) {
  SerialCB* cb = (SerialCB*)data;
  _read_serial_io(cb,cb->io);
  return 0;
}

void start_read_serial_io_(SerialCB* cb, ProcessIo* io) {
  cb->io = io;
  HANDLE thread = CreateThread(NULL, 0, SerialThreadFuncIo, cb, 0, NULL);
  if (thread) {
    // Optionally do stuff, such as wait on the thread.
  }
}

unsigned char write_serial_(SerialCB* cb, const char* buff, int size) {
DWORD dNoOfBytesWritten = 0;     // No of bytes written to the port

BOOL Status = WriteFile(cb->hndl,        // Handle to the Serial port
				buff,     // Data to be written to the port
				size,  //No of bytes to write
                &dNoOfBytesWritten, //Bytes written
                NULL);
return Status;
}



SerialCB* open_serial(int usart, int baud_rate) {
	if (USART1 == usart) {
		return open_ssocket(usart, baud_rate);
	}
	else {
		return open_serial_(usart, baud_rate);
	}
}

void close_serial(SerialCB* cb){
	if (cb->emul) {
		close_ssocket(cb);
	}
	else {
		close_serial_(cb);
	}
}

unsigned char write_serial(SerialCB* cb, const char* buff, int size){
	if (cb->emul) {
		return write_ssocket(cb, buff, size);
	}
	else {
		return write_serial_(cb, buff, size);
	}
}

void start_read_serial_term(SerialCB* cb, TerminalContext* term){
	if (cb->emul) {
		start_read_ssocket(cb, term);
	}
	else {
		start_read_serial_(cb, term);
	}

}


void start_read_serial_io(SerialCB* cb, ProcessIo* io){
	if (cb->emul) {
		//start_read_ssocket_io(cb, io);
	}
	else {
		start_read_serial_io_(cb, io);
	}

}
