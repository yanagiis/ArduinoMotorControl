#ifndef HCODE_H
#define HCODE_H

#include <stdbool.h>
#include <stdint.h>

#include "config.h"

enum HCodeType { HCODE_TYPE_H = 0, HCODE_TYPE_M };
enum ParseError {
    PARSE_ERROR_OK = 0,
    PARSE_ERROR_NO_E,
    PARSE_ERROR_NO_T,
    PARSE_ERROR_NO_S,
    PARSE_ERROR_INVALID_ARG,
    PARSE_ERROR_WRONG_CHECKSUM
};

struct HCode {
    enum HCodeType type;
    struct {
        float water_ml;
        uint8_t available;
    } e[NUM_MOTOR];
    float time_second;
};

void hcode_init(struct HCode *hcode);
enum ParseError parse_hcode(char buf[], uint8_t len, struct HCode *cmd);

#endif  // HCODE_H
