#define F_CPU 8000000


#ifdef __AVR_ATtiny48__
#define DEBUG(x) PORTC = x;
#else
#define DEBUG(x) 
#endif

#include <avr/io.h>
#include <util/delay.h>
#include "SPI.c"

#include "ili9341.c"
#include "SD.c"


#if defined(__AVR_ATtiny48__)
#define N_PIXELS_BUFFERED 64
#elif defined(__AVR_ATtiny84__)
#define N_PIXELS_BUFFERED 128
#endif

#define N_IMAGES 256

uint16_t pixbuf[N_PIXELS_BUFFERED];

void displayInit() {
    ili9341_init();//initial driver setup to drive ili9341
    ili9341_clear(PURPLE);//fill screen with black colour
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

void slideshow() {
    uint16_t current_image = 0;

    while (1) {
        uint16_t pixels_wrote = N_PIXELS_BUFFERED;
        uint32_t current_sector = 0;

        // Read from the first-sector (lookup table of image sector-addresses)
        SD_readSectorN((current_image >= 128) ? 1 : 0, ((current_image) * 4U) % 512U, sizeof(uint32_t), (uint8_t*)&current_sector);
        if (current_image == 0) current_sector += 2;
        // current_sector += 1;

        ili9341_setaddress(0, 0, 320, 240);
        for (uint32_t a = 0; a < (240UL * 320UL); ++a) {
            // Need to read more pixels into the buffer?
            if (pixels_wrote == N_PIXELS_BUFFERED) {
                SD_readSectorRegion(current_sector++, 0, N_PIXELS_BUFFERED * sizeof(uint16_t), (uint8_t*)&pixbuf);
                PORTB ^= _BV(PB0);
                pixels_wrote = 0;
                // DEBUG(current_sector);
            }
            ili9341_pushcolour(pixbuf[pixels_wrote++]);
        }
        // Select a random next image
        current_image = (current_image + 1) % 256;//(((uint8_t)rand()) % (N_IMAGES - 1)) + 1;
        _delay_ms(10 * 1000);
    }
}

int main() {
    CLKPR = 0x80;	// Sets CPU pre-scaler to 1 (8MHz)
    CLKPR = 0x00;	//Second operation in setting CPU pre-scaler to 1

    DDRB |= _BV(PB0);
    PORTB &= ~_BV(PB0);


    // PORTC debugging port
#ifdef __AVR_ATtiny48__
    DDRC = 0xFF;
    PORTC = 0b1;
#endif

    spi_hard_init();
    ili9341_hard_init();
    spi_init();
    SD_hard_init();

    SD_init();
    displayInit();


    slideshow();

    while (1) {}

    return 0;
}