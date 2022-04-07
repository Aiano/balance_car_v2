/**
 * @file pid.h
 * @brief pid控制器
 */

#ifndef BALANCE_CAR_V2_PID_H
#define BALANCE_CAR_V2_PID_H

#include "main.h"
#include <stdio.h>

typedef struct {
    double Kp;
    double Ki;
    double Kd;

    double integral;
    double last_error;

    double max_integral;
    double min_integral;

    double max_u;
    double min_u;
} PID_Datatype;

double pid_get_u(PID_Datatype *pid, double target, double real);

#endif //BALANCE_CAR_V2_PID_H
