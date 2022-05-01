#include <stdint.h>
#include <avr/io.h>

#define SD_CS_PORT PORTD
#define SD_CS_DDR DDRD
#define SD_CS_PIN PD7

// SD card commands
/** GO_IDLE_STATE - init card in spi mode if CS low */
#define SD_CMD0	    (0x40+0)	/* GO_IDLE_STATE */
#define SD_CMD1	    (0x40+1)	/* SEND_OP_COND (MMC) */
#define	SD_ACMD41	(0xC0+41)	/* SEND_OP_COND (SDC) */
#define SD_CMD8	    (0x40+8)	/* SEND_IF_COND */
#define SD_CMD16	(0x40+16)	/* SET_BLOCKLEN */
#define SD_CMD17	(0x40+17)	/* READ_SINGLE_BLOCK */
#define SD_CMD24	(0x40+24)	/* WRITE_BLOCK */
#define SD_CMD55	(0x40+55)	/* APP_CMD */
#define SD_CMD58	(0x40+58)	/* READ_OCR */

void SD_init();

uint8_t SD_readByte();