#include <stdbool.h>
#include <stddef.h>
#include <string.h>
#include "step_command.h"
#include "interrupt.h"

#include "uart.h"

#define MASK (0x7)

void step_command_init(struct StepCommand *cmd)
{
    for (uint8_t i = 0; i < NUM_MOTOR; ++i) {
        cmd[i].step_count = 0;
        cmd[i].interval_tick = 0;
    }
}

void step_command_buffer_init(struct StepCommandBuffer *buf)
{
    memset(buf, 0, sizeof(struct StepCommandBuffer));
}

bool step_command_buffer_is_empty(struct StepCommandBuffer *buf)
{
    uint8_t begin = buf->begin & MASK;
    uint8_t end = buf->end & MASK;

    if (begin == end) {
        return true;
    }
    return false;
}

bool step_command_buffer_is_full(struct StepCommandBuffer *buf)
{
    uint8_t begin = buf->begin & MASK;
    uint8_t end = (buf->end + 1) & MASK;

    if (begin == end) {
        return true;
    }

    return false;
}

struct StepCommand *step_command_buffer_get(struct StepCommandBuffer *buf)
{
    if (step_command_buffer_is_empty(buf)) {
        return NULL;
    }

    struct StepCommand *cmd = buf->commands[buf->begin & MASK];
    buf->begin++;
    return cmd;
}

bool step_command_buffer_put(struct StepCommandBuffer *buf,
                             struct StepCommand *cmd)
{
    if (step_command_buffer_is_full(buf)) {
        return false;
    }

    memcpy(buf->commands[buf->end & MASK], cmd,
           sizeof(struct StepCommand) * NUM_MOTOR);
    buf->end++;

    return true;
}
