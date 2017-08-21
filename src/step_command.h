#ifndef STEP_COMMAND_H
#define STEP_COMMAND_H

#include <stdint.h>
#include <stdbool.h>
#include "config.h"

struct StepCommand {
    uint32_t step_count;
    uint32_t interval_tick;
};

struct StepCommandBuffer {
    struct StepCommand commands[16][NUM_MOTOR];
    uint8_t begin;
    uint8_t end;
};

void step_command_init(struct StepCommand *cmd);
void step_command_buffer_init(struct StepCommandBuffer *buf);
bool step_command_buffer_is_empty(struct StepCommandBuffer *buf);
bool step_command_buffer_is_full(struct StepCommandBuffer *buf);
struct StepCommand *step_command_buffer_get(
    struct StepCommandBuffer *buf);
bool step_command_buffer_put(struct StepCommandBuffer *buf,
                              struct StepCommand *cmd);

#endif  // STEP_COMMAND_H
