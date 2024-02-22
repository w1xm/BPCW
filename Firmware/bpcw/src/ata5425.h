/*
 * ata5425.h
 *
 * Created: 6/4/2023 1:43:40 PM
 *  Author: hansg
 */ 


#ifndef ATA5425_H_
#define ATA5425_H_

	//SPI Pins
	#define TRX_CS PORT_PA18
	#define TRX_RST PORT_PA14
	#define TRX_WAKEUP PORT_PA10
	
	//Registers
	#define TRX_CTRL1 0x0
	#define TRX_CTRL2 0x1
	#define TRX_CTRL3 0x2
	#define TRX_CTRL4 0x3
	#define TRX_CTRL5 0x4
	#define TRX_CTRL6 0x5
	#define TRX_STATUS 0x8
	
	//Bits
	#define TRX_CTRL1_IR1 (1 << 7)
	#define TRX_CTRL1_IR0 (1 << 6)
	#define TRX_CTRL1_AVCC_EN (1 << 5)
	#define TRX_CTRL1_FS (1 << 4)
	#define TRX_CTRL1_OPM_IDLE (0b00 << 1)
	#define TRX_CTRL1_OPM_TX (0b01 << 1)
	#define TRX_CTRL1_OPM_RX_POLL (0b10 << 1)
	#define TRX_CTRL1_OPM_RX (0b11 << 1)
	#define TRX_CTRL1_TMODE (1 << 0)
	
	#define TRX_CTRL2_FR(value) (0b11111110 & ((value) << 1))
	#define TRX_CTRL2_PMODE 0x01
	
	#define TRX_CTRL3_FR(value) (0b11111100 & ((value) << 2))
	#define TRX_CTRL3_VSOUT_EN (1 << 1)
	#define TRX_CTRL3_CKL_ON (1 << 0)
	
	#define TRX_CTRL4_ASK_NFSK (1 << 7)
	#define TRX_CTRL4_SLEEP(value) (0b01111100 & ((value) << 2))
	#define TRX_CTRL4_XSLEEP (1 << 1)
	#define TRX_CTRL4_XLIM (1 << 0)
	
	#define TRX_CTRL5_BITCHK(value) (0b11000000 & ((value) << 6))
	#define TRX_CTRL5_LIM_MIN(value) (0b00111111 & (value))
	
	#define TRX_CTRL6_BAUD(value) (0b11000000 & ((value) << 6))
	#define TRX_CTRL6_LIM_MAX(value) (0b00111111 & ((value)))
	
	#define TRX_STATUS_ST5 (1 << 7)
	#define TRX_STATUS_ST4 (1 << 6)
	#define TRX_STATUS_ST3 (1 << 5)
	#define TRX_STATUS_ST2 (1 << 4)
	#define TRX_STATUS_ST1 (1 << 3)
	#define TRX_STATUS_POWER_ON (1 << 2)
	#define TRX_STATUS_LOW_BATT (1 << 1)
	#define TRX_STATUS_P_ON_AUX (1 << 0)
	
	//Commands
	#define TRX_CMD_READBUF 0b00001111
	#define TRX_CMD_WRITEBUF 0b00101111
	#define TRX_CMD_OFF 0b10001111
	#define TRX_CMD_DEL_IRQ 0b10101111
	
	//function prototypes
	void trx_init(void);
	void trx_cmd_off(void);
	void trx_spi_init(void);
	void trx_spi_deinit(void);
	void trx_spi_write(uint8_t address, uint8_t data);
	uint8_t trx_spi_read(uint8_t address);
	unsigned char check_spi(void);
	
	void clear_rxc(void);
	void trx_transparent_tx(float freq);
	void trx_transparent_rx(float freq);

#endif /* ATA5425_H_ */