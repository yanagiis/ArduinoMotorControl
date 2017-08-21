#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "hcode.h"
#include "uart.h"

static bool parse_htype(char buf[], uint8_t len, struct HCode *cmd);
static bool parse_mtype(char buf[], uint8_t len, struct HCode *cmd);
static bool get_field_value(char buf[],
                            uint8_t len,
                            const char *field,
                            float *v);
static const char *find_str(const char *begin,
                            const char *end,
                            const char *substr);

void hcode_init(struct HCode *hcode)
{
    for (uint8_t i = 0; i < NUM_MOTOR; ++i) {
        hcode->e[i].available = false;
    }
}

bool parse_hcode(char buf[], uint8_t len, struct HCode *cmd)
{
    switch (buf[0]) {
        case 'H':
            cmd->type = HCODE_TYPE_H;
            return parse_htype(buf, len, cmd);
        case 'M':
            cmd->type = HCODE_TYPE_M;
            return parse_mtype(buf, len, cmd);
        default:
            return false;
    }
}

static bool parse_htype(char buf[], uint8_t len, struct HCode *cmd)
{
    float value = 0;
    char field[3] = "E?";

    bool at_least_one_e = false;
    for (uint8_t i = 0; i < NUM_MOTOR; ++i) {
        field[1] = i + '0';
        if (get_field_value(buf, len, field, &value)) {
            cmd->e[i].available = true;
            cmd->e[i].water_ml = value;
            at_least_one_e = true;
        }
    }

    if (!at_least_one_e) {
        uart_puts("No E\r\n");
        return false;
    }

    if (!get_field_value(buf, len, "T", &value)) {
        uart_puts("No T\r\n");
        return false;
    }

    cmd->time_second = value;
    return true;
}

static bool parse_mtype(char buf[], uint8_t len, struct HCode *cmd)
{
    return true;
}

static bool get_field_value(char buf[],
                            uint8_t len,
                            const char *field,
                            float *v)
{
    const char *buf_end = buf + len;
    const char *pos = find_str(buf, buf_end, field);
    if (pos == NULL) {
        return false;
    }

    const char *begin = pos + strlen(field);
    if (begin >= buf_end) {
        return false;
    }

    char *end;
    *v = strtod(begin, &end);
    if (begin == end) {
        return false;
    }
    return true;
}

static const char *find_str(const char *begin,
                            const char *end,
                            const char *substr)
{
    const uint8_t substr_len = strlen(substr);
    if ((end - begin) < substr_len) {
        return NULL;
    }

    end = end - substr_len;
    while (begin != end) {
        if (strncmp(substr, begin, substr_len) == 0) {
            return begin;
        }
        begin++;
    }
    return NULL;
}
