/*
 * storage.c
 *
 *  Created on: 19 gru 2018
 *      Author: lukasz
 */

#include "platform/storage.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void store(int id, const char* buff) {
	FILE *f = fopen("eeprom.txt", "w");
	if (f == NULL)
	{
	    printf("Error opening file!\n");
	    return;
	}
	fprintf(f, "%s", buff);
	fclose(f);
}

#define CHUNK_SIZE 1024
const char* load(int id) {
	char* buf = malloc(CHUNK_SIZE);
	memset(buf,0,CHUNK_SIZE);
	FILE *file;
	size_t nread;

	file = fopen("eeprom.txt", "r");
	if (file) {
	    while ((nread = fread(buf, 1, CHUNK_SIZE, file)) > 0) {

	    }
	    fclose(file);
	}

	return buf;
}
