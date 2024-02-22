/*
 * cw.c
 *
 * Created: 10/25/2022 4:47:21 PM
 *  Author: hansg
 */ 

#include <asf.h>
#include "cw.h"
#include "samd21.h"
#include "ata5425.h"
#include "usart.h"

//Set up the timer/counter for the beeper and the key input pin
void key_init(uint16_t freq){
	
	// Enable and configure generic clock generator 4
	GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC |          // Improve duty cycle
	GCLK_GENCTRL_GENEN |        // Enable generic clock gen
	GCLK_GENCTRL_SRC_OSC8M |  // Select 8MHz
	GCLK_GENCTRL_ID(1);         // Select GCLK4
	while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization

	// Set clock divider of 1 to generic clock generator 4
	GCLK->GENDIV.reg = GCLK_GENDIV_DIV(1) |         // Divide 48 MHz by 1
	GCLK_GENDIV_ID(1);           // Apply to GCLK4 4
	while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
	  
	// Enable GCLK4 and connect it to TCC0 and TCC1
	GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_CLKEN |
		GCLK_CLKCTRL_GEN_GCLK1 |
		GCLK_CLKCTRL_ID(0x25)
	);
	while (GCLK->STATUS.bit.SYNCBUSY);              // Wait for synchronization
	  
	//Enable the TC bus clock
	PM->APBCMASK.reg |= PM_APBCMASK_TCC3;
	
	// set GCLK1 as source to the TCC0 counter
	GCLK->CLKCTRL.reg = (GCLK_CLKCTRL_GEN(1) | GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_ID(GCLK_CLKCTRL_ID_TCC3_Val));
	while(!SYSCTRL->PCLKSR.bit.DFLLRDY);
	
	TCC3->WAVE.reg |= TCC_WAVE_WAVEGEN_NFRQ | TCC_WAVE_POL0;
	while(TCC3->SYNCBUSY.bit.WAVE);
	
	//Set period in PER register
	TCC3->PER.reg = FCLK/freq;
	
	//Set up output pin for beeper
	PORT->Group[0].PINCFG[5].reg |= PORT_PINCFG_PMUXEN; //Enable PMUX
	PORT->Group[0].PMUX[5>>1].bit.PMUXO = PORT_PMUX_PMUXO_F_Val; //Peripheral function F (TCC)
	PORT->Group[0].DIRSET.reg = BEEPER_PIN;
	

	PORT->Group[0].PINCFG[28].reg = PORT_PINCFG_INEN; //Key pin is input
}

void beeper_on(){
	//Turn on the beeper
	TCC3->CTRLA.reg |= TCC_CTRLA_ENABLE;
}

void beeper_off(){
	TCC3->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
}

//Start transmitting a tone
void key_down(){
	trx_transparent_tx(0);
	beeper_on();
}

//Start transmitting a tone
void key_up(){
	trx_transparent_rx(0);
	beeper_off();
}

unsigned char get_rx(){
	/*Take 100 samples of the RX line and see if it's ever high
	unsigned char pin_high = 0;
	for(int i = 0; i < 100; i++){
		pin_high |= ((PORT->Group[0].IN.reg & PORT_PA19) >> 19);
		//delay_us(10);
	}
	*/
	return !check_spi() && (PORT->Group[0].IN.reg & PORT_PA19);
}

unsigned char get_key(){
	return (PORT->Group[0].IN.reg & KEY_PIN) == 0;
}

/*
unsigned char get_rx(){
	return  !check_spi() && (PORT->Group[0].IN.reg & PORT_PA19);
}
*/

void start_cw(){
	trx_init();
	key_init(700);
	
	unsigned char key_down_flag = 0;
	unsigned char rx_flag = 0;
	delay_ms(10);
	while(1){
		
		if(get_key() && !key_down_flag){
			key_down();
			beeper_on();
			key_down_flag = 1;
		}
		
		else if(!get_key() && key_down_flag){
			key_up();
			beeper_off();
			key_down_flag = 0;
		}
		else if(!key_down_flag && get_rx() && !rx_flag){
			beeper_on();
			rx_flag = 1;
		}
		else if(!get_rx() && rx_flag){
			beeper_off();
			rx_flag = 0;
		}
	}
}