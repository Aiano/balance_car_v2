/**
 * @file test_freq.c
 * @brief 用于测试代码块的运行时间
 * @author Aiano_czm
 */

#include "test_freq.h"

uint32_t old_tick = 0;
uint32_t tick_freq = 0;

/**
 * @brief 启动频率检测，在被测代码块运行前调用这个函数
 *
 */
void test_freq_begin() {
    tick_freq = 1000U / HAL_GetTickFreq();
    old_tick = HAL_GetTick();
    return;
}

/**
 * @brief 结束频率检测，在被测代码块运行后调用这个函数
 * @return 代码块运行频率
 */
uint32_t test_freq_end() {
    uint32_t delta_tick = HAL_GetTick() - old_tick;

    uint32_t f = tick_freq / delta_tick;
    return f;
}
