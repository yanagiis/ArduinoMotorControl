#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define TICK_US 128

typedef void(timer_cb)(void *arg);

void timer_init(void);
void timer_enable(void);
void timer_disable(void);
void timer_reg_cb(timer_cb *cb, void *arg);

#endif  // TIMER_H
