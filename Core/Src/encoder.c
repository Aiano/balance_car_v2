/**
 * @file encoder.c
 * @brief 读取电机编码器速度
 */

#include "encoder.h"

float motorA_speed;
float motorB_speed;

/**
 * @brief 初始化编码器和中断的TIM定时器
 */
void encoder_init() {
	//Init TIM
	HAL_TIM_Encoder_Start(ENCODER_A_TIM, TIM_CHANNEL_ALL); // encoderA
    HAL_TIM_Encoder_Start(ENCODER_B_TIM, TIM_CHANNEL_ALL); // encoderB
	HAL_TIM_Base_Start_IT(PERIOD_TIM); // timer:generate 10ms period
}

/**
 * @brief TIM中断回调函数的重载
 * @param period
 */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *period) { // tim callback function
	if (period == PERIOD_TIM) { // PERIOD_TIM: generate 10ms signal

		int16_t A_counts = (int16_t) __HAL_TIM_GET_COUNTER(ENCODER_A_TIM); // ENCODER_A: counts the AB encoder
		int16_t B_counts = (int16_t) __HAL_TIM_GET_COUNTER(ENCODER_B_TIM); // ENCODER_B: counts the AB encoder
		motorA_speed = A_counts;
		motorB_speed = B_counts;
		__HAL_TIM_SET_COUNTER(ENCODER_A_TIM, 0); // ENCODER_A: clear
		__HAL_TIM_SET_COUNTER(ENCODER_B_TIM, 0); // ENCODER_B: clear
	}
}

/**
 * @brief 使用指针获取电机转速
 * @param motorA
 * @param motorB
 */
void encoder_get_speed(float *motorA, float *motorB) {
	*motorA = -motorA_speed; // 加负号是为了统一两个电机的方向
	*motorB = motorB_speed;

    return;
}
