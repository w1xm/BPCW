/**
 * \file
 *
 * \brief Empty user application template
 *
 */

/**
 * \mainpage User Application template doxygen documentation
 *
 * \par Empty user application template
 *
 * Bare minimum empty user application template
 *
 * \par Content
 *
 * -# Include the ASF header files (through asf.h)
 * -# Minimal main function that starts with a call to system_init()
 * -# "Insert application code here" comment
 *
 */

/*
 * Include header files for all drivers that have been imported from
 * Atmel Software Framework (ASF).
 */
/*
 * Support and FAQ: visit <a href="https://www.microchip.com/support/">Microchip Support</a>
 */
#include <asf.h>
#include "main.h"
#include "cw.h"
#include "usart.h"
#include "ata5425.h"


int main (void)
{
	system_init();
	delay_init();
	usart_init();

	PORT->Group[0].DIRSET.reg = PORT_PA06;
	PORT->Group[0].OUT.reg = PORT_PA06;
	
	//beeper_init(800);
	debug_print("Hello World!");
	
	start_cw(); //Start the QSO!
	
	
}
