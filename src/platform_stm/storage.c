/*
 * storage.c
 *
 *  Created on: 15.01.2019
 *      Author: tolek
 */


#include "platform/storage.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stm32f1xx_hal.h>
#include <stm32f1xx_hal_flash_ex.h>
#include "platform/mem.h"

#define DATA_ADDR (0x08010000 - 0x400)

void store(int id, const char* buff) {

	HAL_FLASH_Unlock();

	FLASH_EraseInitTypeDef erase_pages;
	erase_pages.PageAddress = DATA_ADDR;
	erase_pages.NbPages = 1;
	erase_pages.TypeErase = FLASH_TYPEERASE_PAGES;
	uint32_t error;

	HAL_FLASHEx_Erase (&erase_pages, &error);

	uint32_t start_addr = DATA_ADDR;
	while(1) {

		HAL_FLASH_Program (FLASH_TYPEPROGRAM_HALFWORD, start_addr, *buff);
		if (*buff == 0){
			break;
		}
		start_addr += 2;
		buff++;

	}

	HAL_FLASH_Lock();

}

#define MAX_CONFIG 1024
const char* load(int id) {
	uint16_t* data = ((uint16_t *) DATA_ADDR);
	int offset = 0;

	char* result  = port_malloc(MAX_CONFIG);
	memset(result,0,MAX_CONFIG);

	if (data[offset] == 0xffff) {
		// pusty
		return result;
	}

	while(1) {
		result[offset] = data[offset];
		if (data[offset] == 0){
			break;
		}
		offset++;
	}

	return result;
}
