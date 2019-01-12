/*
 * logger.h
 *
 *  Created on: 5 sty 2019
 *      Author: tolek
 */

#ifndef PLATFORM_LOGGER_H_
#define PLATFORM_LOGGER_H_
typedef enum {
	LL_INFO,
	LL_ERROR,
	LL_DEBUG
} LOGGER_LEVEL;

void logger(LOGGER_LEVEL level, const char *format, ...);


#endif /* PLATFORM_LOGGER_H_ */
