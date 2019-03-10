/*
 * pwm.h
 *
 *  Created on: 09.03.2019
 *      Author: tolek
 */

#ifndef INCLUDE_PLATFORM_PWM_H_
#define INCLUDE_PLATFORM_PWM_H_

#define PWM_CH1 0
#define PWM_CH2 1

void set_pwm(int channel, unsigned int value);

#endif /* INCLUDE_PLATFORM_PWM_H_ */
