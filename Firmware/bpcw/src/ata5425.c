/*
 * ata5425.c
 *
 * Created: 6/4/2023 1:43:09 PM
 *  Author: hansg
 */ 

#include <asf.h>

#include "main.h"
#include "ata5425.h"
#include "samd21.h"
#include "usart.h"

void trx_init(){
	PORT->Group[0].DIRCLR.reg = TRX_RST; //This is an INPUT!
	
	PORT->Group[0].OUTSET.reg = TRX_WAKEUP; //Don't wake up yet
	PORT->Group[0].DIRSET.reg = TRX_WAKEUP;
	
	trx_spi_init();
	
	PORT->Group[0].OUTCLR.reg = TRX_WAKEUP; //Wake up
	delay_ms(10);
	PORT->Group[0].OUTSET.reg = TRX_WAKEUP; //Wake up
	
	trx_spi_write(TRX_CTRL6, (TRX_CTRL6_BAUD(0b11) | TRX_CTRL6_LIM_MAX(0b011100)));
	
}

void trx_transparent_tx(float freq){
	trx_spi_write(TRX_CTRL4, TRX_CTRL4_ASK_NFSK | TRX_CTRL4_SLEEP(0b01010));
	trx_spi_write(TRX_CTRL3, TRX_CTRL3_VSOUT_EN | TRX_CTRL3_FR(0b001110));
	trx_spi_write(TRX_CTRL2, TRX_CTRL2_FR(0b1011000));
	trx_spi_write(TRX_CTRL1, TRX_CTRL1_OPM_TX | TRX_CTRL1_TMODE);
	
	//Apply data on SDI
}

void trx_transparent_rx(float freq){
	trx_spi_write(TRX_CTRL5, 0);
	trx_spi_write(TRX_CTRL4, TRX_CTRL4_ASK_NFSK | TRX_CTRL4_SLEEP(0b01010));
	trx_spi_write(TRX_CTRL3, TRX_CTRL3_VSOUT_EN | TRX_CTRL3_FR(0b001110));
	trx_spi_write(TRX_CTRL2, TRX_CTRL2_FR(0b1011000));
	trx_spi_write(TRX_CTRL1, TRX_CTRL1_OPM_RX | TRX_CTRL1_TMODE);
	
	//trx_spi_write(TRX_CTRL5, TRX_CTRL5_BITCHK(1) | TRX_CTRL5_LIM_MIN(0b010000));
	trx_spi_deinit();
}

void trx_spi_init(){
	//initilize SPI on SERCOM1
	
	//Enable Advanced Peripheral Bus Clock for SERCOM1
	REG_PM_APBCMASK |= PM_APBCMASK_SERCOM1;
		
	//Set up Generic Clock Generator 0 for SPI
	GCLK->CLKCTRL.reg = (
		GCLK_CLKCTRL_ID(SERCOM1_GCLK_ID_CORE) |
		GCLK_CLKCTRL_CLKEN |
		GCLK_CLKCTRL_GEN(0)
	);
	
	SERCOM1->USART.CTRLA.reg = ~SERCOM_SPI_CTRLA_ENABLE; //disable the SPI
	
	//Set up IO pins
	PORT->Group[0].PINCFG[16].reg |= PORT_PINCFG_PMUXEN; //Enable PMUX
	PORT->Group[0].PINCFG[17].reg |= PORT_PINCFG_PMUXEN; //Enable PMUX
	PORT->Group[0].PINCFG[19].reg |= PORT_PINCFG_PMUXEN; //Enable PMUX
	
	PORT->Group[0].PMUX[16>>1].bit.PMUXE = PORT_PMUX_PMUXE_C_Val; //Peripheral function C
	PORT->Group[0].PMUX[17>>1].bit.PMUXO = PORT_PMUX_PMUXE_C_Val; //Peripheral function C
	PORT->Group[0].PMUX[19>>1].bit.PMUXO = PORT_PMUX_PMUXO_C_Val; //Peripheral function C
	
	PORT->Group[0].DIRSET.reg = PORT_PA16 | PORT_PA17 | PORT_PA18; //DO, NCS & SCK
	PORT->Group[0].DIRCLR.reg = PORT_PA19;
	PORT->Group[0].OUTCLR.reg = TRX_CS;
	
	//Set operating mode
	//Set PAD[0] = DI
	//Set PAD[1] = SCK
	//Set PAD[2] = NCS
	//Set PAD[3] = DO
	SERCOM1->SPI.CTRLA.reg = (
		SERCOM_SPI_CTRLA_MODE_SPI_MASTER |
		SERCOM_SPI_CTRLA_DIPO(0x3) |
		SERCOM_SPI_CTRLA_DOPO(0x0)
	);
	
	//Receive enable
	SERCOM1->SPI.CTRLB.reg = (
		SERCOM_SPI_CTRLB_RXEN
	);

	SERCOM1->SPI.BAUD.reg = 210; //write baud register slow-ish
	
	//Enable the SPI
	SERCOM1->SPI.CTRLA.reg |= SERCOM_SPI_CTRLA_ENABLE;
}

void trx_spi_deinit(){
	//free the SPI pins
	
	//Set up IO pins
	PORT->Group[0].PINCFG[16].reg &= ~PORT_PINCFG_PMUXEN; //Disable PMUX
	PORT->Group[0].PINCFG[19].reg &= ~PORT_PINCFG_PMUXEN; //Disable PMUX
	
	PORT->Group[0].DIRSET.reg = PORT_PA16 | PORT_PA17 | PORT_PA18; //DO, NCS & SCK
	PORT->Group[0].DIRCLR.reg = PORT_PA19;
	PORT->Group[0].OUTCLR.reg = TRX_CS;

}
void trx_spi_write(uint8_t address, uint8_t data){
	if(!check_spi()){
		trx_spi_init();
	}
	PORT->Group[0].OUTSET.reg = TRX_CS;
	delay_us(10);
	SERCOM1->SPI.DATA.reg = 0b01100000 | address;
	while(!SERCOM1->SPI.INTFLAG.bit.TXC);
	clear_rxc();
	SERCOM1->SPI.DATA.reg = data;
	while(!SERCOM1->SPI.INTFLAG.bit.TXC);
	clear_rxc();
	delay_us(10);
	PORT->Group[0].OUTCLR.reg = TRX_CS;
}

uint8_t trx_spi_read(uint8_t address){
	if(!check_spi()){
		trx_spi_init();
	}
	PORT->Group[0].OUTSET.reg = TRX_CS;
	delay_us(10);
	SERCOM1->SPI.DATA.reg = 0b01000000 | address;
	while(!SERCOM1->SPI.INTFLAG.bit.RXC);
	clear_rxc();
	SERCOM1->SPI.DATA.reg = 0x00;
	while(!SERCOM1->SPI.INTFLAG.bit.RXC);
	delay_us(10);
	PORT->Group[0].OUTCLR.reg = TRX_CS;
	return SERCOM1->SPI.DATA.reg;
}

void trx_cmd_off(){
	if(!check_spi()){
		trx_spi_init();
	}
	PORT->Group[0].OUTSET.reg = TRX_CS;
	delay_us(10);
	SERCOM1->SPI.DATA.reg = TRX_CMD_OFF;
	while(!SERCOM1->SPI.INTFLAG.bit.TXC);
	clear_rxc();
	delay_us(10);
	PORT->Group[0].OUTCLR.reg = TRX_CS;
}

/* helper function for imu_spi_read 
* Reads the SERCOM1 SPI Data register to clear the RXC flag
*/
void clear_rxc(void){
	asm volatile ("" : : "r" (*(unsigned int *)0x42000C28)); //Force a read of the DATA register
}

unsigned char check_spi(){
	return PORT->Group[0].PINCFG[16].bit.PMUXEN && PORT->Group[0].PINCFG[19].bit.PMUXEN;
}