#ifndef STEP_TICK
#define STEP_TICK

#include "step_command.h"

void step_tick_init(struct Motor motors[], struct StepCommandBuffer *buf);
uint32_t time_s_to_tick(float sec);

#endif  // STEP_TICK
