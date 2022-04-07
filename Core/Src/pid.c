/**
 * @file pid.c
 * @brief pid控制器
 */
#include <pid.h>

/**
 * @brief 获取pid的控制量u
 * @param pid
 * @param target
 * @param real
 * @return u
 */
double pid_get_u(PID_Datatype *pid, double target, double real) {
    double error = target - real; // 误差
    pid->integral += error; // 积分误差
    if (pid->integral > pid->max_integral) { // 设置积分误差的上下限，防止过大或过小
        pid->integral = pid->max_integral;
    } else if (pid->integral < pid->min_integral) {
        pid->integral = pid->min_integral;
    }
    double differential = error - pid->last_error; // 差分误差
    //printf("%f\n", differential);

    double u = pid->Kp * error + pid->Ki * pid->integral + pid->Kd * differential;  // 控制量
    if (u > pid->max_u) { // 设置控制量u的上下限，防止过大或过小
        u = pid->max_u;
    } else if (u < pid->min_u) {
        u = pid->min_u;
    }

    pid->last_error = error; // 记录下本轮误差供下次差分使用

    return u;
}

