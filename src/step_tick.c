#include <stdbool.h>
#include <stddef.h>
#include <string.h>

#include "motor.h"
#include "pt/pt.h"
#include "step_tick.h"
#include "timer.h"

struct StepState {
    uint32_t step_count;
    uint32_t interval_tick;
    bool drive;
};

struct StepTickStack {
    struct pt pt;
    struct StepCommandBuffer *buffer;
    struct StepCommand current_command[NUM_MOTOR];
    struct Motor motors[NUM_MOTOR];
    // init in ptthread
    struct StepState state[NUM_MOTOR];
} stack;

static PT_THREAD(step_tick_pt(struct StepTickStack *s));
static void step_tick_cb(void *arg);
static void disable_all_motors(struct Motor motors[], uint8_t num);
static void enable_all_motors(struct Motor motors[], uint8_t num);

void step_tick_init(struct Motor motors[], struct StepCommandBuffer *buf)
{
    PT_INIT(&stack.pt);
    stack.buffer = buf;
    memcpy(stack.motors, motors, sizeof(stack.motors));
    timer_reg_cb(step_tick_cb, (void *)&stack);
}

static void step_tick_cb(void *arg)
{
    struct StepTickStack *s = (struct StepTickStack *)arg;
    step_tick_pt(s);
}

static PT_THREAD(step_tick_pt(struct StepTickStack *s))
{
    PT_BEGIN(&s->pt);
    for (;;) {
        if (step_command_buffer_get(s->buffer, s->current_command) != true) {
            // disable_all_motors(s->motors, NUM_MOTOR);
            PT_YIELD(&s->pt);
            continue;
        }

        for (uint8_t i = 0; i < NUM_MOTOR; ++i) {
            s->state[i].step_count = s->current_command[i].step_count;
            s->state[i].interval_tick = s->current_command[i].interval_tick;
            s->state[i].drive = s->current_command[i].drive;
        }
        enable_all_motors(s->motors, NUM_MOTOR);

        for (;;) {
            bool done = true;
            for (uint8_t i = 0; i < NUM_MOTOR; ++i) {
                done &= (s->state[i].step_count == 0);
            }
            if (done) {
                break;
            }

            for (uint8_t i = 0; i < NUM_MOTOR; ++i) {
                if (s->state[i].step_count == 0) {
                    // skip this motor, wait other motors finish
                    continue;
                }

                if (s->state[i].interval_tick == 0) {
                    s->state[i].interval_tick =
                        s->current_command[i].interval_tick;
                    s->state[i].step_count--;
                    if (s->state[i].drive == 1) {
                        motor_step(&s->motors[i]);
                    }
                } else {
                    s->state[i].interval_tick--;
                }
            }

            PT_YIELD(&s->pt);
        }
    }
    PT_END(&s->pt);
}

static void disable_all_motors(struct Motor motors[], uint8_t num)
{
    for (uint8_t i = 0; i < num; ++i) {
        motor_disable(&motors[i]);
    }
}

static void enable_all_motors(struct Motor motors[], uint8_t num)
{
    for (uint8_t i = 0; i < num; ++i) {
        motor_enable(&motors[i]);
    }
}

uint32_t time_s_to_tick(float sec)
{
    return ((uint32_t) (sec * 1000000) / TICK_US);
}
