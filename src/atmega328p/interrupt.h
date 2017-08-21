#ifndef ATMEGA328P_INTERRUPT_H
#define ATMEGA328P_INTERRUPT_H

#include <avr/interrupt.h>

#define ENABLE_INTERRUPT() sei()
#define DISABLE_INTERRUPT() cli()

#endif  // ATMEGA328P_INTERRUPT_H
