#ifndef MacroAndFunc_H
#define MacroAndFunc_H

#include <avr/io.h>

#define set_OUTPUT_pB(pin) DDRB |= (1<<(pin-8))
#define set_INPUT_pB(pin) DDRB &= ~(1<<(pin-8))
#define write_HIGH_pB(pin) PORTB |= (1<<(pin-8))
#define write_LOW_pB(pin) PORTB &= ~(1<<(pin-8))
#define set_INPUT_A0() DDRC &= ~(1 << PC0)


#define T_set_adr 0
#define T_incalzire_adr 4
#define T_mentinere_adr 8
#define T_racire_adr 12
#define K_P_adr 16
#define K_L_adr 20
#define K_D_adr 24



int read_pB(int pin){
//HIGH - 1 | LOW - 0
if(PINB & (1<<(pin-8)))
  return 1;
return 0;
}



#endif