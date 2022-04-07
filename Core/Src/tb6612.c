/**
 * @file tb6612.c
 * @brief tb6612fng模块底层驱动
 */

#include "tb6612.h"

/**
 * @brief tb6612初始化函数
 * @details 启动PWM_A和PWM_B的TIM定时器
 * @param tb6612
 * @return
 */
uint8_t tb6612_init(TB6612_Datatype *tb6612) {
    HAL_TIM_PWM_Start(tb6612->PWM_A, tb6612->PWM_A_Channel);
    HAL_TIM_PWM_Start(tb6612->PWM_B, tb6612->PWM_B_Channel);
}

/**
 * @brief 设置电机的驱动电压，正负表示方向
 * @param tb6612
 * @param motor_type MOTOR_A 或 MOTOR_B
 * @param value -200 ~ +200
 */
void tb6612_set_value(TB6612_Datatype *tb6612, MOTOR_Type motor_type, int16_t value) {
    // 选择电机
    GPIO_TypeDef *Port1;
    uint16_t Pin1;
    GPIO_TypeDef *Port2;
    uint16_t Pin2;
    TIM_HandleTypeDef *PWM;
    uint32_t PWM_Channel;
    if (motor_type == MOTOR_A) {
        Port1 = tb6612->AIN1_port;
        Pin1 = tb6612->AIN1_pin;
        Port2 = tb6612->AIN2_port;
        Pin2 = tb6612->AIN2_pin;
        PWM = tb6612->PWM_A;
        PWM_Channel = tb6612->PWM_A_Channel;
    } else {
        Port1 = tb6612->BIN1_port;
        Pin1 = tb6612->BIN1_pin;
        Port2 = tb6612->BIN2_port;
        Pin2 = tb6612->BIN2_pin;
        PWM = tb6612->PWM_B;
        PWM_Channel = tb6612->PWM_B_Channel;
    }

    // 设置电机方向
    if (value > 0) {
        HAL_GPIO_WritePin(Port1, Pin1, GPIO_PIN_SET);
        HAL_GPIO_WritePin(Port2, Pin2, GPIO_PIN_RESET);
    } else {
        value = -value;
        HAL_GPIO_WritePin(Port1, Pin1, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(Port2, Pin2, GPIO_PIN_SET);
    }

    // 设置驱动电压
    __HAL_TIM_SET_COMPARE(PWM, PWM_Channel, value);

    return;
}