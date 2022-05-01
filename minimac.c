#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>

#include "ili9341.c"
#include "SD.c"

// void tft_drawimage(const char* fname) {
//     // ili9341_setaddress(x, y, x + w - 1, y + h - 1);

//     // for (y = h; y > 0; y--) {
//     //     for (x = w; x > 0; x--) {
//     //         ili9341_pushcolour(colour);
//     //     }
//     // }
// }

int main() {
    CLKPR = 0x80;	// Sets CPU pre-scaler to 1 (8MHz)
    CLKPR = 0x00;	//Second operation in setting CPU pre-scaler to 1

    spi_init();
    SD_CS_DDR |= _BV(SD_CS_PIN);
    SD_CS_high(); // Unselect SD card

    // PORTC debugging port
    DDRC = 0xFF;
    PORTC = 0b1;

    SD_init();

    // Happy
    // PORTC = 0x55;
    // while (1) {
    //     PORTC ^= 0xFF;
    //     _delay_ms(200);
    // }

    //displayProg();

    displayInit();

    // uint8_t idx = 0;
    // while (idx++ < 0b111111) {
    //     _delay_ms(100);
    //     PORTC = idx;
    //     SD_readSectorHeader(idx);
    // }


    PORTC = SD_readSectorHeader(0);
    for (uint16_t y = 0; y < 240; ++y) {
        for (uint16_t x = 0; x < 320; ++x) {
            uint16_t sdpix = SD_readSectorHeader((uint32_t)y * 320 + (uint32_t)x);
            ili9341_drawpixel(x, y, sdpix);
        }
        PORTC = y;
    }
    while (1) {}


    return 0;
}

void displayInit() {
    ili9341_init();//initial driver setup to drive ili9341
    ili9341_clear(BLUE);//fill screen with black colour
    _delay_ms(1000);
    ili9341_setRotation(3);//rotate screen
    _delay_ms(2);
}

void displayProg() {
    displayInit();

#define LINES 20

    while (1) {
        for (uint16_t v = 0; v < LINES; ++v) {
            ili9341_drawbresenham(
                0,
                (v * ILI9341_TFTHEIGHT) / LINES,
                ((LINES - v) * ILI9341_TFTWIDTH) / LINES,
                0,
                RED
            );

            ili9341_drawbresenham(
                ILI9341_TFTWIDTH,
                ((LINES - v) * ILI9341_TFTHEIGHT) / LINES,
                (v * ILI9341_TFTWIDTH) / LINES,
                ILI9341_TFTHEIGHT,
                RED
            );
        }
        //PORTC = (PORTC + 1) & 0b111111;
        _delay_ms(100);
    }
}

void initADC() {
    // AREF = AVcc
    ADMUX = (1 << REFS0);

    // ADC Enable and prescaler of 128
    // 16000000/128 = 125000
    ADCSRA = (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);
}

uint16_t readADC(uint8_t ch) {
    // select the corresponding channel 0~7
    // ANDing with ’7′ will always keep the value
    // of ‘ch’ between 0 and 7
    ch &= 0b00000111;  // AND operation with 7
    ADMUX = (ADMUX & 0xF8) | ch; // clears the bottom 3 bits before ORing

    // start single convertion
    // write ’1′ to ADSC
    ADCSRA |= (1 << ADSC);

    // wait for conversion to complete
    // ADSC becomes ’0′ again
    // till then, run loop continuously
    while (ADCSRA & (1 << ADSC));

    return (ADC);
}