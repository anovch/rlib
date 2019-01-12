/*
 * storage.h
 *
 *  Created on: 19 gru 2018
 *      Author: lukasz
 */

#ifndef PLATFORM_STORAGE_H_
#define PLATFORM_STORAGE_H_

void store(int id, const char* buff);
const char* load(int id);

#endif /* PLATFORM_STORAGE_H_ */
