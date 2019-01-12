/*
 * ssocket.c
 *
 *  Created on: 5 sty 2019
 *      Author: tolek
 */

#define WINVER 0x0A00
#define _WIN32_WINNT 0x0A00

#include "ssocket.h"
#include <w32api.h>
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

#define DEFAULT_BUFLEN 512
#define DEFAULT_PORT "1122"

static void logloc(const char *format, ...) {
	va_list args;
	va_start(args, format);
	vprintf(format, args);
	va_end(args);
	fflush(stdout);
}

SerialCB* open_ssocket(int usart, int baud_rate) {
	SerialCB* result;
	result = (SerialCB*) malloc(sizeof(SerialCB));

	result->hndl = malloc(sizeof(SOCKET));
	result->usr = malloc(sizeof(SOCKET));

	WSADATA wsaData;
	int iResult;

	SOCKET ListenSocket = INVALID_SOCKET;

	struct addrinfo *addrinforesult = NULL;
	struct addrinfo hints;

	// Initialize Winsock
	iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
	if (iResult != 0) {
		logloc("WSAStartup failed with error: %d\n", iResult);
		return result;
	}

	ZeroMemory(&hints, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	hints.ai_flags = AI_PASSIVE;

	// Resolve the server address and port
	iResult = getaddrinfo(NULL, DEFAULT_PORT, &hints, &addrinforesult);
	if (iResult != 0) {
		logloc("getaddrinfo failed with error: %d\n", iResult);
		WSACleanup();
		return result;
	}

	// Create a SOCKET for connecting to server
	ListenSocket = socket(addrinforesult->ai_family,
			addrinforesult->ai_socktype, addrinforesult->ai_protocol);
	if (ListenSocket == INVALID_SOCKET) {
		logloc("socket failed with error: %ld\n", WSAGetLastError());
		freeaddrinfo(addrinforesult);
		WSACleanup();
		return result;
	}

	// Setup the TCP listening socket
	iResult = bind(ListenSocket, addrinforesult->ai_addr,
			(int) addrinforesult->ai_addrlen);
	if (iResult == SOCKET_ERROR) {
		logloc("bind failed with error: %d\n", WSAGetLastError());
		freeaddrinfo(addrinforesult);
		closesocket(ListenSocket);
		WSACleanup();
		return result;
	}

	freeaddrinfo(addrinforesult);

	iResult = listen(ListenSocket, SOMAXCONN);
	if (iResult == SOCKET_ERROR) {
		logloc("listen failed with error: %d\n", WSAGetLastError());
		closesocket(ListenSocket);
		WSACleanup();
		return result;
	}

	logloc("Init serial emul socket OK\n");

	*((SOCKET*) result->hndl) = ListenSocket;
	result->emul = 1;
	return result;
}

void close_ssocket(SerialCB* cb) {
	int iResult;
	SOCKET ClientSocket = *((SOCKET*) cb->usr);

	iResult = shutdown(ClientSocket, SD_SEND);
	if (iResult == SOCKET_ERROR) {
		logloc("shutdown failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return;
	}

	// cleanup
	closesocket(ClientSocket);
	WSACleanup();
}

unsigned char write_ssocket(SerialCB* cb, const char* buff, int size) {
	int iSendResult;
	SOCKET ClientSocket = *((SOCKET*) cb->usr);

	iSendResult = send(ClientSocket, buff, size, 0);
	if (iSendResult == SOCKET_ERROR) {
		logloc("send failed with error: %d\n", WSAGetLastError());
		closesocket(ClientSocket);
		WSACleanup();
		return -1;
	}
	return 0;

}


static void recv_socket(SerialCB* cb, TerminalContext* term) {
	int iResult;
	SOCKET ClientSocket = *((SOCKET*) cb->usr);

//	const char loc_echo_off[] = {0x0FF, 0x0FD, 0x2D};
//	write_serial(cb, loc_echo_off, 3);
	terminal_set_echo(term,0);

	BufferOutput* buffout = buffer_init(256);
	terminal_get_header(term, buffout);
	write_serial(cb, buffer_get(buffout), strlen(buffer_get(buffout)));

	do {
		char TempChar;
		iResult = recv(ClientSocket, &TempChar, sizeof(TempChar), 0);

		if (iResult > 0) {
			//write_serial(hComm, &TempChar,NoBytesRead);
			test_malloc();
			terminal_input(term, &TempChar, iResult, buffout);
			int size = buffer_size(buffout);
			if (size != 0) {
				write_serial(cb, buffer_get(buffout), size);
			}
		} else if (iResult == 0)
			logloc("Connection closing...\n");
		else {
			logloc("recv failed with error: %d\n", WSAGetLastError());
			closesocket(ClientSocket);
			WSACleanup();
			return;
		}

	} while (iResult > 0);

}

static void _read_socket(SerialCB* cb, TerminalContext* term) {

	SOCKET ListenSocket = *((SOCKET*) cb->hndl);
	// Accept a client socket
	SOCKET ClientSocket = INVALID_SOCKET;
	while (1) {
		ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			logloc("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return;
		}
		*((SOCKET*) cb->usr) = ClientSocket;
		recv_socket(cb, term);
	}
}

static DWORD WINAPI ScoketThreadFunc(void* data) {

	SerialCB* cb = (SerialCB*) data;
	_read_socket(cb, cb->term);

	return 0;
}

void start_read_ssocket(SerialCB* cb, TerminalContext* term) {

	cb->term = term;
	HANDLE thread = CreateThread(NULL, 0, ScoketThreadFunc, cb, 0, NULL);
	if (thread) {
		// Optionally do stuff, such as wait on the thread.
	}
}
