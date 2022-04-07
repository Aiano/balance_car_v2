/**
 * @file encoder.h
 * @brief 读取电机编码器速度
 */

#ifndef INC_ENCODER_H_
#define INC_ENCODER_H_

#include "main.h"
#include "tim.h"
#include <stdio.h>

#define ENCODER_A_TIM &htim2
#define ENCODER_B_TIM &htim4
#define PERIOD_TIM &htim3

void encoder_init();
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *period);
void encoder_get_speed(float *motorA, float *motorB);

#endif /* INC_ENCODER_H_ */
