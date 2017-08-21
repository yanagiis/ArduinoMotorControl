#ifndef ATMEGA328P_GPIO_H
#define ATMEGA328P_GPIO_H

#include <stdint.h>

enum GPIO_PORT { GPIO_PORT_B = 1, GPIO_PORT_C, GPIO_PORT_D };
enum GPIO_DIR { GPIO_DIR_INPUT, GPIO_DIR_OUTPUT };

#define GPIO_LOW 0
#define GPIO_HIGH 1

struct GPIO {
    enum GPIO_PORT port;
    uint8_t pin;
};

#define GPIO_INIT(_port, _pin)     \
    {                              \
        .port = _port, .pin = _pin \
    }

void gpio_set_dir(const struct GPIO *gpio, enum GPIO_DIR dir);
uint8_t gpio_get_value(const struct GPIO *gpio);
void gpio_set_value(const struct GPIO *gpio, uint8_t value);
void gpio_toggle_value(const struct GPIO *gpio);

#endif  // ATMEGA328P_GPIO_H
