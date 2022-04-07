/**
 * @file tb6612.h
 * @brief tb6612fng模块底层驱动
 */

#ifndef BALANCE_CAR_V2_TB6612_H
#define BALANCE_CAR_V2_TB6612_H

#include "main.h"

/**
 * @brief tb6612初始化结构体
 */
typedef struct {
    GPIO_TypeDef *AIN1_port;
    uint16_t AIN1_pin;
    GPIO_TypeDef *AIN2_port;
    uint16_t AIN2_pin;
    TIM_HandleTypeDef *PWM_A;
    uint32_t PWM_A_Channel;

    GPIO_TypeDef *BIN1_port;
    uint16_t BIN1_pin;
    GPIO_TypeDef *BIN2_port;
    uint16_t BIN2_pin;
    TIM_HandleTypeDef *PWM_B;
    uint32_t PWM_B_Channel;
}TB6612_Datatype;

/**
 * @brief A或B电机
 */
typedef enum {
    MOTOR_A = 0,
    MOTOR_B
}MOTOR_Type;

uint8_t tb6612_init(TB6612_Datatype *tb6612);
void tb6612_set_value(TB6612_Datatype *tb6612, MOTOR_Type motor_type, int16_t value);

#endif //BALANCE_CAR_V2_TB6612_H
