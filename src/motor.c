#include "motor.h"
#include "gpio.h"

void motor_init(const struct Motor *motor)
{
    gpio_set_dir(&motor->enable, GPIO_DIR_OUTPUT);
    gpio_set_dir(&motor->dir, GPIO_DIR_OUTPUT);
    gpio_set_dir(&motor->step, GPIO_DIR_OUTPUT);
}

void motor_enable(const struct Motor *motor)
{
    gpio_set_value(&motor->enable, 0);
}

void motor_disable(const struct Motor *motor)
{
    gpio_set_value(&motor->enable, 1);
}

void motor_dir(const struct Motor *motor, enum MotorDir dir)
{
    switch (dir) {
        case MOTOR_DIR_CLOCKWISE:
            gpio_set_value(&motor->dir, 1);
            break;
        case MOTOR_DIR_COUNTER_CLOCKWISE:
            gpio_set_value(&motor->dir, 0);
            break;
    }
}

void motor_step(const struct Motor *motor)
{
    gpio_toggle_value(&motor->step);
}
