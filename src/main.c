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

#define WATER_PER_PULSE (0.0027475)

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
                (uint32_t)((hcode->e[i].water_ml / WATER_PER_PULSE) * 2);
            if (cmd[i].step_count >= 2) {
                cmd[i].interval_tick = time_s_to_tick(hcode->time_second) /
                                       (cmd[i].step_count / 2);
                cmd[i].drive = 1;
            } else {
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
        {GPIO_INIT(GPIO_PORT_D, 1), GPIO_INIT(GPIO_PORT_D, 4),
         GPIO_INIT(GPIO_PORT_D, 5)},
        {GPIO_INIT(GPIO_PORT_D, 1), GPIO_INIT(GPIO_PORT_D, 2),
         GPIO_INIT(GPIO_PORT_D, 3)},
    };

    board_init();

    // motors init
    step_command_buffer_init(&step_command_buffer);
    for (uint8_t i = 0; i < NUM_MOTOR; ++i) {
        motor_init(&motors[i]);
        motor_dir(&motors[i], MOTOR_DIR_CLOCKWISE);
    }

    step_tick_init(motors, &step_command_buffer);

    // enable timer interrupt
    timer_enable();


#if 0
    struct StepCommand cmd[2];
    for (;;) {
        step_command_init(cmd);
        cmd[0].drive = true;
        cmd[0].interval_tick = 15625;
        cmd[0].step_count = 80000;
        cmd[1].drive = false;
        cmd[1].interval_tick = 100;
        cmd[1].step_count = 800;

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
                break;
        }
    }

    return 0;
}
