#include <avr/io.h>
#include "gpio.h"

#define PORT_B_DIR 0x24
#define PORT_B_IO 0x25
#define PORT_C_DIR 0x27
#define PORT_C_IO 0x28
#define PORT_D_DIR 0x2A
#define PORT_D_IO 0x2B

static volatile uint8_t *get_port_dir_mem(enum GPIO_PORT port)
{
    switch (port) {
        case GPIO_PORT_B:
            return (uint8_t *)PORT_B_DIR;
        case GPIO_PORT_C:
            return (uint8_t *)PORT_C_DIR;
        case GPIO_PORT_D:
            return (uint8_t *)PORT_D_DIR;
        default:
            return (uint8_t *)0;
    }
}

static volatile uint8_t *get_port_io_mem(enum GPIO_PORT port)
{
    switch (port) {
        case GPIO_PORT_B:
            return (uint8_t *)PORT_B_IO;
        case GPIO_PORT_C:
            return (uint8_t *)PORT_C_IO;
        case GPIO_PORT_D:
            return (uint8_t *)PORT_D_IO;
        default:
            return (uint8_t *)0;
    }
}

void gpio_set_dir(const struct GPIO *gpio, enum GPIO_DIR dir)
{
    volatile uint8_t *mem_dir = get_port_dir_mem(gpio->port);
    uint8_t v = _BV(gpio->pin);
    switch (dir) {
        case GPIO_DIR_INPUT:
            *mem_dir &= ~v;
            break;
        case GPIO_DIR_OUTPUT:
            *mem_dir |= v;
            break;
    }
}

uint8_t gpio_get_value(const struct GPIO *gpio)
{
    volatile uint8_t *mem_io = get_port_io_mem(gpio->port);
    return (*mem_io & _BV(gpio->pin))? GPIO_HIGH: GPIO_LOW;
}

void gpio_set_value(const struct GPIO *gpio, uint8_t value)
{
    volatile uint8_t *mem_io = get_port_io_mem(gpio->port);
    uint8_t v = _BV(gpio->pin);
    switch (value) {
        case GPIO_HIGH:
            *mem_io |= v;
            break;
        case GPIO_LOW:
            *mem_io &= ~v;
            break;
    }
}

void gpio_toggle_value(const struct GPIO *gpio)
{
    volatile uint8_t *mem_io = get_port_io_mem(gpio->port);
    uint8_t v = _BV(gpio->pin);
    *mem_io ^= v;
}
