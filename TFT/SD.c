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
    uint8_t crc;

    /* Send command packet */
    spi_send(cmd);						    /* Start + Command index */
    spi_send((uint8_t)(arg >> 24));		    /* Argument[31..24] */
    spi_send((uint8_t)(arg >> 16));		    /* Argument[23..16] */
    spi_send((uint8_t)(arg >> 8));			/* Argument[15..8] */
    spi_send((uint8_t)arg);				    /* Argument[7..0] */
    crc = 0x01;							    /* Dummy CRC + Stop */
    if (cmd == SD_CMD0) crc = 0x95;			// CRC for CMD0(0)
    if (cmd == SD_CMD8) crc = 0x87;			// CRC for CMD8(0x1AA)
    if (cmd == SD_CMD58) crc = 0xFD;        // CRC for CMD58(0)
    if (cmd == SD_CMD55) crc = 0x65;        // CRC for CMD55(0)
    if (cmd == SD_ACMD41) crc = 0x77;        // CRC for ACMD41(0x40000000)
    return spi_transaction(crc);            // Send CRC
}

// uint32_t SD_send_cmd_res32(uint8_t cmd, uint32_t arg) {
//     uint8_t crc, rshiftctr;
//     uint32_t result;

//     /* Send command packet */
//     spi_send(cmd);						    /* Start + Command index */
//     spi_transaction((uint8_t)(arg >> 24));		    /* Argument[31..24] */
//     spi_transaction((uint8_t)(arg >> 16));		    /* Argument[23..16] */
//     spi_transaction((uint8_t)(arg >> 8));			/* Argument[15..8] */
//     spi_transaction((uint8_t)arg);				    /* Argument[7..0] */
//     crc = 0x01;							    /* Dummy CRC + Stop */
//     if (cmd == SD_CMD0) crc = 0x95;			// CRC for CMD0(0)
//     if (cmd == SD_CMD8) crc = 0x87;			// CRC for CMD8(0x1AA)
//     if (cmd == SD_CMD58) crc = 0xFD;        // CRC for CMD58(0)
//     spi_transaction(crc);                   // Send CRC
//     return result;
// }

void SD_CS_high() {
    SD_CS_PORT |= _BV(SD_CS_PIN);
}

void SD_CS_low() {
    SD_CS_PORT &= ~_BV(SD_CS_PIN);
}

void SD_waitForResponse(uint8_t* response, uint8_t target) {
    while (*response != target) {
        *response = spi_transaction(0xFF); // Keep asking for response
        //_delay_us(1);
    }
}

void SD_init() {
    // Wait
    _delay_ms(1);
    SD_CS_high();

    // Clock pulses (12*8)
    for (uint8_t i = 0; i < 12; ++i) {
        spi_send(0xFF);
    }

    // Software reset
    SD_CS_low();

    // Ask for idle state
    uint8_t response = SD_send_cmd(SD_CMD0, 0);
    SD_waitForResponse(&response, 0x1);


    PORTC = 0b100; // Card in idle


    response = SD_send_cmd(SD_CMD8, 0x1AA);
    uint32_t cmd8resData;
    while (response != 0x1) {
        if (response == 0x05 || response == 0x0D) goto skipCMD8; // Old card, unsupported command
        response = spi_transaction(0xFF); // Keep asking for response
        _delay_us(10);
    }
    // Read 4 byte response word
    for (uint8_t i = 4; i > 0; --i) {
        cmd8resData |= spi_transaction(0xFF) << ((i - 1) * 8);
    }
skipCMD8:


    PORTC = 0b1000;


    uint32_t OCR;
    response = SD_send_cmd(SD_CMD58, 0);
    SD_waitForResponse(&response, 0x1);
    // Read OCR
    for (uint8_t i = 4; i > 0; --i) {
        OCR |= spi_transaction(0xFF) << ((i - 1) * 8);
    }
    // Check for voltage range
    // if ((OCR & 0x00380000) == 0x00380000)
    //     //PORTC = 0b111111;
    // else return;


    PORTC = 0b10000;


step6:
    // Send CMD55 to ready the card for a application-specific command
    response = SD_send_cmd(SD_CMD55, 0);
    while (response != 0x1 && response != 0x0) {
        response = spi_transaction(0xFF); // Keep asking for response
        _delay_us(10);
    }


    PORTC = 0b100000;


    response = SD_send_cmd(SD_ACMD41, 0x40000000);
    uint8_t timeout = 50;
    while (response != 0x0 && timeout--) {
        if (response == 0x1) goto step6;
        if (timeout == 1) {
            response = SD_send_cmd(SD_CMD1, 0x40000000);
            timeout = 50;
        };
        response = spi_transaction(0xFF); // Keep asking for response
        _delay_us(10);
    }


    PORTC = 0b110000;


    response = SD_send_cmd(SD_CMD1, 0x40000000);
    while (response != 0x0) {
        if (response == 0x1) goto step6;
        response = spi_transaction(0xFF); // Keep asking for response
        _delay_us(10);
    }


    PORTC = 0xFF;


    spi_send(0xFF);
    SD_CS_high(); // Done using SD for now
    spi_send(0xFF); // Finish it
}

void SD_startSectorRead(uint32_t sector) {
    // Select SD
    spi_send(0xFF);
    SD_CS_low();
    spi_send(0xFF);

    uint8_t resp = SD_send_cmd(SD_CMD17, sector);
    SD_waitForResponse(&resp, 0x00);   // Wait for R1 response
    SD_waitForResponse(&resp, 0xFE);    // Wait for 0xFE "start token"
    // spi_send(0xFF);
    // spi_send(0xFF);
    // spi_send(0xFF);
    // spi_send(0xFF);
}

uint8_t SD_readByteSector() {
    return spi_transaction(0xFF);
}

void SD_endSectorRead() {
    // Clear out the CRC and send an extra for good measure
    spi_send(0xFF);
    spi_send(0xFF);
    spi_send(0xFF);


    // Disable SD card
    SD_CS_high();
    spi_send(0xFF);
}

uint16_t SD_readSectorHeader(uint32_t sector) {
    uint16_t res = 0;
    SD_startSectorRead(sector);
    res |= SD_readByteSector();
    res |= (uint16_t)SD_readByteSector() << 8;
    uint16_t sbl = SD_SECTOR_SIZE - 2;
    while (sbl--) spi_send(0xFF); // Read rest of sector
    SD_endSectorRead();
    return res;
}

void SD_readSectorPartial(uint32_t sector, uint16_t start, uint16_t end, uint8_t* buf) {
    SD_startSectorRead(sector);
    for (uint16_t s = start; s; --s) SD_readByteSector();
    for (uint16_t r = 0; r < (end - start); ++r) buf[r] = SD_readByteSector();
    for (uint16_t s = end; s < 512; ++s) SD_readByteSector();
    SD_endSectorRead();
}

void SD_hard_init() {
    SD_CS_DDR |= _BV(SD_CS_PIN);
    SD_CS_high(); // Unselect SD card
}