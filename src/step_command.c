#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "step_command.h"

#define MASK (STEP_COMMAND_BUFFER - 1)

void step_command_init(struct StepCommand *cmd)
{
    for (uint8_t i = 0; i < NUM_MOTOR; ++i) {
        cmd[i].step_count = 0;
        cmd[i].interval_tick = 0;
        cmd[i].drive = 0;
    }
}

void step_command_buffer_init(struct StepCommandBuffer *buf)
{
    memset(buf, 0, sizeof(struct StepCommandBuffer));
}

bool step_command_buffer_is_empty(struct StepCommandBuffer *buf)
{
    uint8_t begin = buf->begin;
    uint8_t end = buf->end;

    if (begin == end) {
        return true;
    }
    return false;
}

bool step_command_buffer_is_full(struct StepCommandBuffer *buf)
{
    uint8_t begin = buf->begin;
    uint8_t end = (buf->end + 1) & MASK;

    if (begin == end) {
        return true;
    }

    return false;
}

bool step_command_buffer_get(struct StepCommandBuffer *buf,
                             struct StepCommand *cmd)
{
    if (step_command_buffer_is_empty(buf)) {
        return false;
    }

    memcpy(cmd, buf->commands[buf->begin],
           sizeof(struct StepCommand) * NUM_MOTOR);
    buf->begin = (buf->begin + 1) & MASK;
    return true;
}

bool step_command_buffer_put(struct StepCommandBuffer *buf,
                             struct StepCommand *cmd)
{
    if (step_command_buffer_is_full(buf)) {
        return false;
    }

    memcpy(buf->commands[buf->end], cmd,
           sizeof(struct StepCommand) * NUM_MOTOR);
    buf->end = (buf->end + 1) & MASK;

    return true;
}
