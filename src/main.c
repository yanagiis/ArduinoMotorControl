#include "config.h"
#include "gpio.h"
#include "hcode.h"
#include "motor.h"
#include "step_command.h"
#include "step_tick.h"
#include "timer.h"
#include "uart.h"
#include "stdio.h"
#include <util/delay.h>
#include <stddef.h>

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
        if (c == '\r' || c == '\n') {
            buf[len++] = 0;
            break;
        }
    }
    return len;
}

static bool hcode_to_step_command(struct HCode *hcode, struct StepCommand *cmd)
{
    for (uint8_t i = 0; i < NUM_MOTOR; ++i) {
        if (hcode->e[i].available) {
            uint32_t ml_to_count = (uint32_t)hcode->e[i].water_ml;
            cmd[i].step_count = ml_to_count << 1;
            cmd[i].interval_tick =
                time_s_to_tick(hcode->time_second) / ml_to_count;
        }
    }
    return true;
}

int main(void)
{
    struct StepCommandBuffer step_command_buffer;
    struct Motor motors[NUM_MOTOR] = {
        {GPIO_INIT(GPIO_PORT_D, 2), GPIO_INIT(GPIO_PORT_D, 3),
         GPIO_INIT(GPIO_PORT_D, 4)},
        {GPIO_INIT(GPIO_PORT_D, 2), GPIO_INIT(GPIO_PORT_D, 5),
         GPIO_INIT(GPIO_PORT_D, 6)},
    };

    board_init();

    // motors init
    step_command_buffer_init(&step_command_buffer);
    for (uint8_t i = 0; i < NUM_MOTOR; ++i) {
        motor_init(&motors[i]);
    }

    step_tick_init(motors, &step_command_buffer);

    // enable timer interrupt
    timer_enable();

#if 0
    struct StepCommand cmd[2];
    cmd[0].interval_tick = 100;
    cmd[0].step_count = 400;
    cmd[1].interval_tick = 100;
    cmd[1].step_count = 400;
    step_command_buffer_put(&step_command_buffer, cmd);
    for (;;) {
        _delay_ms(10);
    }
#endif

// wait command from uart
    uart_puts("Start\r\n");

    for (;;) {
        char uart_buffer[UART_COMMAND_BUFFER_SIZE];
        uint8_t len = uart_readline(uart_buffer, sizeof(uart_buffer));
        if (len == sizeof(uart_buffer) && uart_buffer[len - 1] != 0) {
            continue;
        }

        if (uart_buffer[0] == 0) {
            uart_puts("OK\n");
            continue;
        }

        struct HCode hcode;
        hcode_init(&hcode);
        if (!parse_hcode(uart_buffer, len, &hcode)) {
            uart_puts("Error\n");
            continue;
        }

        struct StepCommand cmd[2];
        step_command_init(cmd);
        switch (hcode.type) {
            case HCODE_TYPE_H:
                if (hcode_to_step_command(&hcode, cmd)) {
                    step_command_buffer_put(&step_command_buffer, cmd);
                    uart_puts("OK\n");
                } else {
                    uart_puts("Error\n");
                }
                break;
            case HCODE_TYPE_M:
                break;
        }
        _delay_ms(1000);
    }

    return 0;
}
