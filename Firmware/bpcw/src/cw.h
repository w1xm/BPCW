/*
 * cw.h
 *
 * Created: 10/25/2022 4:47:38 PM
 *  Author: hansg
 */ 
#include <stdint.h>

#ifndef CW_H_
#define CW_H_

//System clock
#define FCLK 6000000UL

#define GCLK_CLKCTRL_ID_TCC3_Val 0x25

#define BEEPER_PIN PORT_PA05
#define KEY_PIN PORT_PA28

//Function prototypes
void key_init(uint16_t freq);
void key_down(void);
void key_up(void);
void beeper_off(void);
void beeper_on(void);
unsigned char get_key(void);
unsigned char get_rx(void);
void start_cw(void);


#endif /* CW_H_ */