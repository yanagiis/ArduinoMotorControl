#ifndef MOTOR_H
#define MOTOR_H

#include "gpio.h"

enum MotorDir { MOTOR_DIR_CLOCKWISE, MOTOR_DIR_COUNTER_CLOCKWISE };

struct Motor {
    struct GPIO enable;
    struct GPIO dir;
    struct GPIO step;
};

void motor_init(const struct Motor *motor);
void motor_dir(const struct Motor *motor, enum MotorDir dir);
void motor_step(const struct Motor *motor);
void motor_enable(const struct Motor *motor);
void motor_disable(const struct Motor *motor);

#endif  // MOTOR_H
