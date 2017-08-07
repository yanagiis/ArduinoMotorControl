/* 
 * File: blink.c
 * Description: Toggels pin PB0 every 500ms
 * From: C-Programmierung mit AVR-GCC
 */

#define F_CPU 20000000UL

#include <avr/io.h>
#include <util/delay.h>

int main (void) {

   DDRD |= (1 << PD2);
   DDRD |= (1 << PD3);

   while(1) {
       PORTD ^= (1 << PD3);
       _delay_us(300);
   }

   return 0;
}
