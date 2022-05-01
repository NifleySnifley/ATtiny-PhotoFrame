#include "SD.h"
#include <util/delay.h>

uint8_t spi_transaction(uint8_t data) {
    SPDR = data; // Data to TX in SPDR
    while (!(SPSR & (1 << SPIF))); // Wait for completion
    return SPDR; // RX'd data is back in SPDR
}

void spi_send(uint8_t data) {
    SPDR = data; // Data to TX in SPDR
    while (!(SPSR & (1 << SPIF))); // Wait for completion
}

uint8_t SD_send_cmd(uint8_t cmd, uint32_t arg) {
    uint8_t n, res;

    /* Send command packet */
    spi_send(cmd);						/* Start + Command index */
    spi_send((uint8_t)(arg >> 24));		/* Argument[31..24] */
    spi_send((uint8_t)(arg >> 16));		/* Argument[23..16] */
    spi_send((uint8_t)(arg >> 8));			/* Argument[15..8] */
    spi_send((uint8_t)arg);				/* Argument[7..0] */
    n = 0x01;							/* Dummy CRC + Stop */
    if (cmd == SD_CMD0) n = 0x95;			/* Valid CRC for CMD0(0) */
    //if (cmd == SD_CMD8) n = 0x87;			/* Valid CRC for CMD8(0x1AA) */
    return spi_transaction(n);
}

void SD_CS_high() {
    SD_CS_PORT |= _BV(SD_CS_PIN);
}

void SD_CS_low() {
    SD_CS_PORT &= ~_BV(SD_CS_PIN);
}

void SD_init() {
    // Wait
    _delay_ms(10);
    SD_CS_high();

    // Clock pulses (12*8)
    for (uint8_t i = 0; i < 12; ++i) {
        spi_send(0xFF);
    }

    // Software reset
    SD_CS_low();

    // Ask for idle state
    uint8_t errmsg = SD_send_cmd(SD_CMD0, 0);

    while (errmsg != 0x1) {
        errmsg = spi_transaction(0xFF); //SD_send_cmd(SD_CMD0, 0);
        PORTC = errmsg;
        _delay_ms(1);
    }

    PORTC = 0b100;


}