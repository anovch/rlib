/*
 * pwm.c
 *
 *  Created on: 09.03.2019
 *      Author: tolek
 */
#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include "platform/pwm.h"

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;



static void set_pwm_(unsigned int value, TIM_HandleTypeDef* htim) {
  TIM_OC_InitTypeDef sConfigOC;

  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = value;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  sConfigOC.OCIdleState = TIM_OCIDLESTATE_RESET;
  sConfigOC.OCNIdleState = TIM_OCNIDLESTATE_RESET;
  if (HAL_TIM_PWM_ConfigChannel(htim, &sConfigOC, TIM_CHANNEL_4) != HAL_OK)
  {
    _Error_Handler(__FILE__, __LINE__);
  }
  HAL_TIM_PWM_Start(htim,TIM_CHANNEL_4);

}


//TIM_OCStructInit(&channel);
//channel.TIM_OCMode = TIM_OCMode_PWM1;
//channel.TIM_OutputState = TIM_OutputState_Enable;
//channel.TIM_Pulse = 100;
//TIM_OC1Init(TIM4, &channel);

void set_pwm(int channel, unsigned int value) {
	if (channel == PWM_CH1) {
		set_pwm_(value, &htim1);
	}
	if (channel == PWM_CH2) {
		set_pwm_(value, &htim2);
	}

}

void set_dir(int channel, unsigned int value) {
	GPIO_TypeDef* port = DIR1_GPIO_Port;
	uint16_t pin = DIR1_Pin;
	if (channel != 0) {
		port = DIR2_GPIO_Port;
		pin = DIR2_Pin;
	}
	if (value == 0) {
		HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET);
	}
	else {
		HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET);
	}
}
