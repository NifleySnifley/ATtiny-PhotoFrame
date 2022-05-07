#include "ili9341.h"
#include "ili9341cmd.h"
#include <stdlib.h>

volatile uint16_t LCD_W = ILI9341_TFTWIDTH;
volatile uint16_t LCD_H = ILI9341_TFTHEIGHT;

//init hardware
void ili9341_hard_init(void) {
    // DDRB |= _BV(1) | _BV(2);
    ili9341_controlddr |= _BV(ili9341_cs) | _BV(ili9341_dc);
    ili9341_controlport |= _BV(ili9341_cs); // CS high
    ili9341_rstddr |= _BV(ili9341_rst);//output for reset
    ili9341_rstport |= (1 << ili9341_rst);//pull high for normal operation
}

//command write
void ili9341_writecommand8(uint8_t com) {
    ili9341_controlport &= ~((1 << ili9341_dc) | (1 << ili9341_cs));//ili9341_dc and ili9341_cs both low to send command
    _delay_us(5);//little delay
    spi_send(com);
    ili9341_controlport |= (1 << ili9341_cs);//pull high ili9341_cs
}

//data write
void ili9341_writedata8(uint8_t data) {
    ili9341_controlport |= (1 << ili9341_dc);//st ili9341_dc high for data
    _delay_us(1);//delay
    ili9341_controlport &= ~(1 << ili9341_cs);//set ili9341_cs low for operation
    spi_send(data);
    ili9341_controlport |= (1 << ili9341_cs);
}

//set coordinate for print or other function
void ili9341_setaddress(uint16_t x1, uint16_t y1, uint16_t x2, uint16_t y2) {
    ili9341_writecommand8(ILI9341_CMD_COLUMN_ADDRESS_SET);
    ili9341_writedata8(x1 >> 8);
    ili9341_writedata8(x1);
    ili9341_writedata8(x2 >> 8);
    ili9341_writedata8(x2);

    ili9341_writecommand8(ILI9341_CMD_PAGE_ADDRESS_SET);
    ili9341_writedata8(y1 >> 8);
    ili9341_writedata8(y1);
    ili9341_writedata8(y2);
    ili9341_writedata8(y2);

    ili9341_writecommand8(ILI9341_CMD_MEMORY_WRITE);//meory write
}

//hard reset display
void ili9341_hard_reset(void) {
    ili9341_rstport |= (1 << ili9341_rst);//pull high if low previously
    _delay_ms(200);
    ili9341_rstport &= ~(1 << ili9341_rst);//low for reset
    _delay_ms(200);
    ili9341_rstport |= (1 << ili9341_rst);//again pull high for normal operation
    _delay_ms(200);
}

//set up display using predefined command sequence
void ili9341_init(void) {
    ili9341_hard_init();
    //spi_init();
    ili9341_hard_reset();
    ili9341_writecommand8(ILI9341_CMD_SOFTWARE_RESET);//soft reset
    _delay_ms(1000);
    //power control A
    ili9341_writecommand8(ILI9341_CMD_POWER_ON_SEQ_CONTROL);
    ili9341_writedata8(0x39);
    ili9341_writedata8(0x2C);
    ili9341_writedata8(0x00);
    ili9341_writedata8(0x34);
    ili9341_writedata8(0x02);

    //power control B
    ili9341_writecommand8(ILI9341_CMD_POWER_CONTROL_B);
    ili9341_writedata8(0x00);
    ili9341_writedata8(0xC1);
    ili9341_writedata8(0x30);

    //driver timing control A
    ili9341_writecommand8(ILI9341_CMD_DRIVER_TIMING_CONTROL_A);
    ili9341_writedata8(0x85);
    ili9341_writedata8(0x00);
    ili9341_writedata8(0x78);

    //driver timing control B
    ili9341_writecommand8(ILI9341_CMD_DRIVER_TIMING_CONTROL_B);
    ili9341_writedata8(0x00);
    ili9341_writedata8(0x00);

    //power on sequence control
    ili9341_writecommand8(ILI9341_CMD_POWERON_SEQ_CTL);
    ili9341_writedata8(0x64);
    ili9341_writedata8(0x03);
    ili9341_writedata8(0x12);
    ili9341_writedata8(0x81);

    //pump ratio control
    ili9341_writecommand8(ILI9341_CMD_PUMP_RATIO_CONTROL);
    ili9341_writedata8(0x20);

    //power control,VRH[5:0]
    ili9341_writecommand8(ILI9341_CMD_POWER_CONTROL_1);
    ili9341_writedata8(0x23);

    //Power control,SAP[2:0];BT[3:0]
    ili9341_writecommand8(ILI9341_CMD_POWER_CONTROL_2);
    ili9341_writedata8(0x10);

    //vcm control
    ili9341_writecommand8(ILI9341_CMD_VCOM_CONTROL_1);
    ili9341_writedata8(0x3E);
    ili9341_writedata8(0x28);

    //vcm control 2
    ili9341_writecommand8(ILI9341_CMD_VCOM_CONTROL_2);
    ili9341_writedata8(0x86);

    //memory access control
    ili9341_writecommand8(ILI9341_CMD_MEMORY_ACCESS_CONTROL);
    ili9341_writedata8(0x48);

    //pixel format
    ili9341_writecommand8(ILI9341_CMD_COLMOD_PIXEL_FORMAT_SET);
    ili9341_writedata8(0x55);

    //frameration control,normal mode full colours
    ili9341_writecommand8(ILI9341_CMD_FRAME_RATE_CONTROL_NORMAL);
    ili9341_writedata8(0x00);
    ili9341_writedata8(0x18);

    //display function control
    ili9341_writecommand8(ILI9341_CMD_DISPLAY_FUNCTION_CONTROL);
    ili9341_writedata8(0x08);
    ili9341_writedata8(0x82);
    ili9341_writedata8(0x27);

    //3gamma function disable
    ili9341_writecommand8(ILI9341_CMD_ENABLE_3_GAMMA_CONTROL);
    ili9341_writedata8(0x00);

    //gamma curve selected
    ili9341_writecommand8(ILI9341_CMD_GAMMA_SET);
    ili9341_writedata8(0x01);

    //set positive gamma correction
    ili9341_writecommand8(ILI9341_CMD_POSITIVE_GAMMA_CORRECTION);
    ili9341_writedata8(0x0F);
    ili9341_writedata8(0x31);
    ili9341_writedata8(0x2B);
    ili9341_writedata8(0x0C);
    ili9341_writedata8(0x0E);
    ili9341_writedata8(0x08);
    ili9341_writedata8(0x4E);
    ili9341_writedata8(0xF1);
    ili9341_writedata8(0x37);
    ili9341_writedata8(0x07);
    ili9341_writedata8(0x10);
    ili9341_writedata8(0x03);
    ili9341_writedata8(0x0E);
    ili9341_writedata8(0x09);
    ili9341_writedata8(0x00);

    //set negative gamma correction
    ili9341_writecommand8(ILI9341_CMD_NEGATIVE_GAMMA_CORRECTION);
    ili9341_writedata8(0x00);
    ili9341_writedata8(0x0E);
    ili9341_writedata8(0x14);
    ili9341_writedata8(0x03);
    ili9341_writedata8(0x11);
    ili9341_writedata8(0x07);
    ili9341_writedata8(0x31);
    ili9341_writedata8(0xC1);
    ili9341_writedata8(0x48);
    ili9341_writedata8(0x08);
    ili9341_writedata8(0x0F);
    ili9341_writedata8(0x0C);
    ili9341_writedata8(0x31);
    ili9341_writedata8(0x36);
    ili9341_writedata8(0x0F);

    //exit sleep
    ili9341_writecommand8(ILI9341_CMD_SLEEP_OUT);
    _delay_ms(120);
    //display on
    ili9341_writecommand8(ILI9341_CMD_DISPLAY_ON);

}

//set colour for drawing
void ili9341_pushcolour(uint16_t colour) {
    ili9341_writedata8(colour >> 8);
    ili9341_writedata8(colour);
}


//clear lcd and fill with colour
void ili9341_clear(uint16_t colour) {
    uint16_t i, j;
    ili9341_setaddress(0, 0, LCD_W - 1, LCD_H - 1);

    for (i = 0;i < LCD_W;i++) {
        for (j = 0;j < LCD_H;j++) {
            ili9341_pushcolour(colour);
        }
    }

}

// Integer signum
int16_t sign(int16_t val) {
    return (0 < val) - (val < 0);
}

// Draws a line from (x0, y0) to (x1, y1) using Bresenham's algorithm
void ili9341_drawbresenham(uint16_t x0, uint16_t y0, uint16_t x1, uint16_t y1, uint16_t colour) {
    // TODO: Optimize using sign bit majik
    int16_t dx = abs(x1 - x0);
    int16_t sx = sign(x1 - x0);
    int16_t dy = -abs(y1 - y0);
    int16_t sy = sign(y1 - y0);
    int16_t error = dx + dy;

    while (1) {
        ili9341_drawpixel(x0, y0, colour);
        if (x0 == x1 && y0 == y1) break; // Done
        int16_t e2 = 2 * error;
        if (e2 >= dy) {
            if (x0 == x1) break;
            error += dy;
            x0 += sx;
        }
        if (e2 < dx) {
            if (y0 == y1) break;
            error += dx;
            y0 += sy;
        }
    }
}

//draw pixel
 //pixels will always be counted from right side.x is representing LCD width which will always be less tha 240.Y is representing LCD height which will always be less than 320
void ili9341_drawpixel(uint16_t x3, uint16_t y3, uint16_t colour1) {
    if ((x3 < 0) || (x3 >= LCD_W) || (y3 < 0) || (y3 >= LCD_H)) return;

    ili9341_setaddress(x3, y3, x3 + 1, y3 + 1);

    ili9341_pushcolour(colour1);
}


//draw vertical line
//basically we will see this line horizental if we see the display 320*240
void ili9341_drawvline(uint16_t x, uint16_t y, uint16_t h, uint16_t colour) {
    if ((x >= LCD_W) || (y >= LCD_H)) return;
    if ((y + h - 1) >= LCD_H)
        h = LCD_H - y;
    ili9341_setaddress(x, y, x, y + h - 1);
    while (h--) {
        ili9341_pushcolour(colour);
    }
}


//draw horizental line
void ili9341_drawhline(uint16_t x, uint16_t y, uint16_t w, uint16_t colour) {
    if ((x >= LCD_W) || (y >= LCD_H)) return;
    if ((x + w - 1) >= LCD_W)
        w = LCD_W - x;
    ili9341_setaddress(x, y, x + w - 1, y);
    while (w--) {
        ili9341_pushcolour(colour);
    }
}


//draw colour filled rectangle
void ili9341_fillrect(uint16_t x, uint16_t y, uint16_t w, uint16_t h, uint16_t colour) {
    if ((x >= LCD_W) || (y >= LCD_H)) return;
    if ((x + w - 1) >= LCD_W)
        w = LCD_W - x;
    if ((y + h - 1) >= LCD_H)
        h = LCD_H - y;

    ili9341_setaddress(x, y, x + w - 1, y + h - 1);

    for (y = h; y > 0; y--) {
        for (x = w; x > 0; x--) {
            ili9341_pushcolour(colour);
        }
    }
}

//rotate screen at desired orientation
void ili9341_setRotation(uint8_t m) {
    uint8_t rotation;
    ili9341_writecommand8(ILI9341_CMD_MEMORY_ACCESS_CONTROL);
    rotation = m % 4;
    switch (rotation) {
        case 0:
            ili9341_writedata8(0x40 | 0x08);
            LCD_W = 240;
            LCD_H = 320;
            break;
        case 1:
            ili9341_writedata8(0x20 | 0x08);
            LCD_W = 320;
            LCD_H = 240;
            break;
        case 2:
            ili9341_writedata8(0x80 | 0x08);
            LCD_W = 240;
            LCD_H = 320;
            break;
        case 3:
            ili9341_writedata8(0x40 | 0x80 | 0x20 | 0x08);
            LCD_W = 320;
            LCD_H = 240;
            break;
    }
}