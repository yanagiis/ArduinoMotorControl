#ifndef STEP_COMMAND_H
#define STEP_COMMAND_H

#include <stdbool.h>
#include <stdint.h>
#include "config.h"

#define STEP_COMMAND_BUFFER 4

#if STEP_COMMAND_BUFFER & (STEP_COMMAND_BUFFER - 1) != 0
#error "STEP_COMMAND_BUFFER is not power of two"
#endif

struct StepCommand {
    uint32_t step_count;
    uint32_t interval_tick;
    bool drive;
};

struct StepCommandBuffer {
    uint8_t begin;
    uint8_t end;
    struct StepCommand commands[STEP_COMMAND_BUFFER][NUM_MOTOR];
};

void step_command_init(struct StepCommand *cmd);
void step_command_buffer_init(struct StepCommandBuffer *buf);
bool step_command_buffer_is_empty(struct StepCommandBuffer *buf);
bool step_command_buffer_is_full(struct StepCommandBuffer *buf);
bool step_command_buffer_get(struct StepCommandBuffer *buf, struct StepCommand *cmd);
bool step_command_buffer_put(struct StepCommandBuffer *buf,
                             struct StepCommand *cmd);

#endif  // STEP_COMMAND_H
