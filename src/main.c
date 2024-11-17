#include <stddef.h>
#include <util/delay.h>

#include "config.h"
#include "gpio.h"
#include "hcode.h"
#include "motor.h"
#include "stdio.h"
#include "step_command.h"
#include "step_tick.h"
#include "timer.h"
#include "uart.h"

float water_per_pulse[NUM_MOTOR] = {0.0027475, 0.0027475};

static void board_init(void)
{
    uart_init(UART_BAUD_SELECT(57600, F_CPU));
    timer_init();
}

static uint8_t uart_readline(char buf[], int size)
{
    int len;
    for (len = 0; len < size - 1; ++len) {
        uint16_t c;
        for (c = uart_getc(); c == UART_NO_DATA; c = uart_getc())
            ;
        buf[len] = (char)c;
        if (c == '\r') {
            buf[len] = 0;
            len -= 1;
        } else if (c == '\n') {
            buf[len] = 0;
            break;
        }
    }
    return len + 1;
}

static bool hcode_to_step_command(struct HCode *hcode, struct StepCommand *cmd)
{
    for (uint8_t i = 0; i < NUM_MOTOR; ++i) {
        if (hcode->e[i].available) {
            cmd[i].step_count =
                (uint32_t)((hcode->e[i].water_ml / water_per_pulse[i]) * 2);
            if (cmd[i].step_count >= 2) {
                cmd[i].interval_tick = time_s_to_tick(hcode->time_second) /
                                       (cmd[i].step_count / 2);
                cmd[i].drive = 1;
            } else {
                cmd[i].step_count = 0;
                cmd[i].interval_tick = time_s_to_tick(hcode->time_second);
                cmd[i].drive = 0;
            }
        }
    }
    return true;
}

int main(void)
{
    struct StepCommandBuffer step_command_buffer;
    struct Motor motors[NUM_MOTOR] = {
        {
            .enable = GPIO_INIT(GPIO_PORT_B, 0),
            .dir = GPIO_INIT(GPIO_PORT_D, 3),
            .step = GPIO_INIT(GPIO_PORT_D, 2),
        },
        {
            .enable = GPIO_INIT(GPIO_PORT_B, 1),
            .dir = GPIO_INIT(GPIO_PORT_D, 5),
            .step = GPIO_INIT(GPIO_PORT_D, 4),
        },
    };

    board_init();

    // motors init
    step_command_buffer_init(&step_command_buffer);
    for (uint8_t i = 0; i < NUM_MOTOR; ++i) {
        motor_init(&motors[i]);
    }

    motor_dir(&motors[0], MOTOR_DIR_COUNTER_CLOCKWISE);
    motor_dir(&motors[1], MOTOR_DIR_COUNTER_CLOCKWISE);

    step_tick_init(motors, &step_command_buffer);

    // enable timer interrupt
    timer_enable();

#if 0
    struct StepCommand cmd[2];
    for (;;) {
        step_command_init(cmd);
        cmd[0].drive = true;
        cmd[0].interval_tick = 1562;
        cmd[0].step_count = 80000;
        cmd[1].drive = true;
        cmd[1].interval_tick = 1562;
        cmd[1].step_count = 80000;

        while (step_command_buffer_put(&step_command_buffer, cmd) != true)
            ;

        while (true);
    }
#endif

    uart_puts("start\r\n");
    for (;;) {
        char uart_buffer[UART_COMMAND_BUFFER_SIZE];
        uint8_t len = uart_readline(uart_buffer, sizeof(uart_buffer));

        if (len == sizeof(uart_buffer) && uart_buffer[len - 1] != 0) {
            continue;
        }

        if (uart_buffer[0] == 0) {
            uart_puts("ok\r\n");
            continue;
        }

        struct HCode hcode;
        enum ParseError error;
        hcode_init(&hcode);
        if ((error = parse_hcode(uart_buffer, len, &hcode)) != PARSE_ERROR_OK) {
            switch (error) {
                case PARSE_ERROR_NO_E:
                    uart_puts("No E\r\n");
                    break;
                case PARSE_ERROR_NO_T:
                    uart_puts("No T\r\n");
                    break;
                case PARSE_ERROR_NO_S:
                    uart_puts("No S\r\n");
                    break;
                case PARSE_ERROR_INVALID_ARG:
                    uart_puts("Invalid args\r\n");
                    break;
                case PARSE_ERROR_WRONG_CHECKSUM:
                    uart_puts("Invalid checksum\r\n");
                    break;
                default:
                    break;
            }
            continue;
        }

        struct StepCommand cmd[2];
        step_command_init(cmd);
        switch (hcode.type) {
            case HCODE_TYPE_H:
                if (hcode_to_step_command(&hcode, cmd)) {
                    while (step_command_buffer_put(&step_command_buffer, cmd) !=
                           true)
                        ;
                    uart_puts("ok\r\n");
                } else {
                    uart_puts("hcode to step error\r\n");
                }
                break;
            case HCODE_TYPE_M:
                for (int i = 0; i < NUM_MOTOR; ++i) {
                    water_per_pulse[i] = hcode.e[i].water_ml;
                }
                uart_puts("ok\r\n");
                break;
        }
    }

    return 0;
}
