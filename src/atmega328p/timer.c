#include <avr/io.h>
#include <stddef.h>
#include "interrupt.h"
#include "timer.h"

struct {
    timer_cb *cb;
    void *arg;
} context = {NULL, NULL};

void timer_init(void)
{
    DISABLE_INTERRUPT();
    DDRD |= (1 << PORTD7);

    TCCR0A &= ~(_BV(COM0A1) | _BV(COM0A0));
    TCCR0A |= _BV(WGM01);
    TIMSK0 |= _BV(OCIE0A);
    OCR0A = 0xF;
    ENABLE_INTERRUPT();
}

void timer_enable(void)
{
    DISABLE_INTERRUPT();
    TCCR0B &= ~(_BV(CS02) | _BV(CS01) | _BV(CS00));
    TCCR0B |= (_BV(CS00) | _BV(CS01));
    ENABLE_INTERRUPT();
}

void timer_disable(void)
{
    DISABLE_INTERRUPT();
    TCCR0B &= ~(_BV(CS02) | _BV(CS01) | _BV(CS00));
    ENABLE_INTERRUPT();
}

void timer_reg_cb(timer_cb *cb, void *arg)
{
    DISABLE_INTERRUPT();
    context.arg = arg;
    context.cb = cb;
    ENABLE_INTERRUPT();
}

ISR(TIMER0_COMPA_vect)
{
    PORTD ^= (1 << PORTD7);
    if (context.cb) {
        context.cb(context.arg);
    }
}
