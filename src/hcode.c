#include "hcode.h"

#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "uart.h"

static enum ParseError parse_htype(char buf[], uint8_t len, struct HCode *cmd);
static enum ParseError parse_mtype(char buf[], uint8_t len, struct HCode *cmd);
static bool get_field_hex(char buf[], uint8_t len, const char *field, int *i);
static bool get_field_float(char buf[],
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

enum ParseError parse_hcode(char buf[], uint8_t len, struct HCode *cmd)
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

static enum ParseError verify_checksum(char buf[], uint8_t len)
{
    int calc_checksum = 0;
    int cmd_checksum = 0;
    if (!get_field_hex(buf, len, "S", &cmd_checksum)) {
        return PARSE_ERROR_NO_S;
    }

    for (uint8_t i = 0; i < len; ++i) {
        if (buf[i] == 'S') {
            break;
        }
        calc_checksum += buf[i];
    }

    if (calc_checksum != cmd_checksum) {
        return PARSE_ERROR_WRONG_CHECKSUM;
    }

    return PARSE_ERROR_OK;
}

static enum ParseError parse_htype(char buf[], uint8_t len, struct HCode *cmd)
{
    enum ParseError err = verify_checksum(buf, len);
    if (err != PARSE_ERROR_OK) {
        return err;
    }

    float value = 0;
    char field[3] = "E?";
    bool at_least_one_e = false;

    for (uint8_t i = 0; i < NUM_MOTOR; ++i) {
        field[1] = i + '0';
        if (get_field_float(buf, len, field, &value)) {
            cmd->e[i].available = true;
            cmd->e[i].water_ml = value;
            at_least_one_e = true;
        }
    }

    if (!get_field_float(buf, len, "T", &value)) {
        return PARSE_ERROR_NO_T;
    }

    cmd->time_second = value;
    return PARSE_ERROR_OK;
}

static enum ParseError parse_mtype(char buf[], uint8_t len, struct HCode *cmd)
{
    enum ParseError err = verify_checksum(buf, len);
    if (err != PARSE_ERROR_OK) {
        return err;
    }

    float value = 0;
    char field[3] = "E?";

    for (uint8_t i = 0; i < NUM_MOTOR; ++i) {
        field[1] = i + '0';
        if (get_field_float(buf, len, field, &value)) {
            cmd->e[i].available = true;
            cmd->e[i].water_ml = value;
        }
    }

    return PARSE_ERROR_OK;
}

static bool get_field_hex(char buf[], uint8_t len, const char *field, int *i)
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
    *i = strtol(begin, &end, 16);
    if (begin == end) {
        return false;
    }
    return true;
}

static bool get_field_float(char buf[],
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
