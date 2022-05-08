#include <stdint.h>
#include <avr/io.h>
#include <util/atomic.h>


#if defined(__AVR_ATtiny48__)

// CS and DC/C
#define ili9341_controlport PORTB 
#define ili9341_controlddr DDRB
#define ili9341_controlpin PINB
#define ili9341_cs 1
#define ili9341_dc 0

// HW reset
#define ili9341_rstport PORTD
#define ili9341_rstddr DDRD
#define ili9341_rstpin PIND
#define ili9341_rst 6

#define SPI_DDR DDRB
#define SPI_MOSI PB3
#define SPI_MISO PB4
#define SPI_CLK PB5

#elif defined(__AVR_ATtiny84__)

// CS and DC/C
#define ili9341_controlport PORTA 
#define ili9341_controlddr DDRA
#define ili9341_controlpin PINA
#define ili9341_cs 1
#define ili9341_dc 2

// HW reset
#define ili9341_rstport PORTA
#define ili9341_rstddr DDRA
#define ili9341_rstpin PINA
#define ili9341_rst 0

#define SPI_DDR DDRA
#define SPI_MOSI PA5
#define SPI_MISO PA6
#define SPI_CLK PA4

#endif


#if defined(__AVR_ATtiny48__)

// Init HW SPI master
void spi_init(void) {
    // DDRB |= _BV(3) | _BV(5);//CS,SS,MOSI,SCK as output(although SS will be unused throughout the program)
    SPCR = (1 << SPE) | (1 << MSTR);//mode 0,fosc/4
    SPSR |= (1 << SPI2X);//doubling spi speed.i.e final spi speed-fosc/2
}

uint8_t spi_transaction(uint8_t data) {
    spi_send(data);
    return SPDR; // RX'd data is back in SPDR
}

void spi_send(uint8_t data) {
    SPDR = data; // Data to TX in SPDR
    while (!(SPSR & (1 << SPIF))); // Wait for completion
}

#elif defined(__AVR_ATtiny84__)

//#define ICRSET _BV(USITC) | _BV(USIWM0) | _BV(USICS0)
#define	USI_CLK_L ((1 << USIWM0) | (1 << USITC) | (0 << USICS0) | (1 << USIOIE))
#define	USI_CLK_H (USI_CLK_L | (1 << USICLK))

// Init USI SPI master
void spi_init(void) {
    //USICR = ICRSET;
    sei();
}

void spi_send(uint8_t data) {
    // USIDR = data;
    // USISR = (1 << USIOIF);

    // USICR = USI_CLK_L;
    // USICR = USI_CLK_H;

    // USICR = USI_CLK_L;
    // USICR = USI_CLK_H;

    // USICR = USI_CLK_L;
    // USICR = USI_CLK_H;

    // USICR = USI_CLK_L;
    // USICR = USI_CLK_H;

    // USICR = USI_CLK_L;
    // USICR = USI_CLK_H;

    // USICR = USI_CLK_L;
    // USICR = USI_CLK_H;

    // USICR = USI_CLK_L;
    // USICR = USI_CLK_H;

    // USICR = USI_CLK_L;
    // USICR = USI_CLK_H;

    USIDR = data;
    USISR = (1 << USIOIF);
    do {
        USICR = (1 << USIWM0) | (1 << USICS1) | (1 << USICLK) | (1 << USITC);
    } while ((USISR & (1 << USIOIF)) == 0);
    return USIDR;
}

uint8_t spi_transaction(uint8_t data) {
    // TODO:
    spi_send(data);
    return USIDR;
}

#endif

void spi_hard_init() {
    SPI_DDR |= _BV(SPI_CLK) | _BV(SPI_MOSI);
    SPI_DDR &= ~_BV(SPI_MISO);
}