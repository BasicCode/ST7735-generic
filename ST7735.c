/*
 * File:   ST7735.c
 * Author: tommy
 *
 * Basic functionality of the ST7735 TFT LCD controller.
 * Designed for PIC 16 series micros over SPI interface but I will
 * try to keep it as general as possible. This will not be a complete
 * library, it's just intended as a starting point to build upon depending
 * on the device and project needs.
 * 
 * In order to use this library please set the chip select, reset, etc.
 * pin definitions in the header file. Also please write your own SPI
 * initialisation function because it differs from device to device.
 * 
 * The basic theory of operation of these ST77xx chips is:
 * - Initial power-on-reset. Individual initialisation routine should be
 * added as required.
 * 
 * - Memory is written in user-defined blocks ("windows"). You can tell the
 * device which memory area (display area) you want to change,
 * and then just send a stream of colour values (2 byte each pixel) to fill
 * in the space.
 * 
 * Created on 17 October 2018
 */

//#include <math.h>
#include <xc.h>
#include "ST7735.h"



/*
 * Writes a byte to SPI without changing chip select (CSX) state.
 * Called by the write_command() and write_data() functions which
 * control these pins as required.
 * 
 * This will use either a software implementation or the hardware
 * implementation depending on if USE_HW_SPI flag is set in the 
 * header file. (Software SPI is reeeeeally slow. Measured at 
 * 40 kHz clock compared to 2 MHz clock with hardware SPI).
 */
void spi_write(unsigned char data) {
    if(USE_HW_SPI) {
        //Use the on-bard hardware SPI registers
        //TODO: Update these buffer labels according to your device.
        
        //Write data to SSPBUFF
        SSP1BUF = data;
        //Wait for transmission to finish
        while(!SSP1STAT & 0x01);
    } else {
        //Otherwise just bit bang this through
        for(int i = 7; i >= 0; i--) {
            //asm("NOP"); //May or may not be needed for timing
            SCK = 0;
            SDO = (data >> i) & 0x01;
            //asm("NOP");
            SCK = 1; //Data sampled on rising clock edge.
        }
    }
}

/*
 * Writes a data byte to the display. Pulls CS low as required.
 */
void lcd_write_data(unsigned char data) {
    //CS LOW
    CSX = 0;
    //Send data to the SPI register
    spi_write(data);
    //CS HIGH
    CSX = 1;
}

/*
 * Writes a command byte to the display
 */
void lcd_write_command(unsigned char data) {
    //Pull the command AND chip select lines LOW
    DCX = 0;
    CSX = 0;
    spi_write(data);
    //Return the control lines to HIGH
    DCX = 1;
    CSX = 1;
}

/*
 * Delay calcualted on 8MHz clock.
 * Does NOT adjust to clock setting
 */
void delay_ms(long int millis) {
    unsigned long int cycles = millis * 1000;
    while(millis--);
}

/*
 * A short microsecond delay routine
 * (not measured)
 */
void delay_us(long int cycles) {
    while(cycles--);
}

/*
 * Initialisation routine for the LCD
 * Thanks to Adafruit for the magic numbers here;
 * Buy their stuff here
 * https://www.adafruit.com/product/2088
 */
void lcd_init() {
    
    //SET control pins for the LCD HIGH (they are active LOW)
    CSX = 1; //CS
    DCX = 1; //Data / command select, the datasheet isn't clear on that.
    RESX = 1; //RESET pin HIGH
    
    //Cycle reset pin
    RESX = 0;
    delay_ms(500);
    RESX = 1;
    delay_ms(500);
    
    lcd_init_command_list();
}

/**
 * After a bit of trial and error with the libraries made by Adafruit and others
 * I have settled on this. You may want to add your own settings to the 
 * command list here.
 */
void lcd_init_command_list(void)
{
    
    lcd_write_command(ST7735_SWRESET);
    delay_ms(100);
    lcd_write_command(0x11);//Sleep out
    delay_ms(120);
    //------//

    //Add any custom settings to the command list here
    
    //------//
    lcd_write_command(0x3A);
    lcd_write_data(0x05);
    
    lcd_write_command(0x29);//Display on
}

/*
 * Draws a single pixel to the LCD at position X, Y, with 
 * Colour.
 */
void draw_pixel(unsigned char x, unsigned char y, unsigned int colour) {
    //Set the x, y position that we want to write to
    set_draw_window(x, y, x+1, y+1);
    lcd_write_data(colour >> 8);
    lcd_write_data(colour & 0xFF);
}

/*
 * Fills a rectangle with a given colour
 */
void fill_rectangle(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int colour) {
    //Split the colour int in to two bytes
    unsigned char colour_high = colour >> 8;
    unsigned char colour_low = colour & 0xFF;
    
    //Set the drawing region
    set_draw_window(x1, y1, x2, y2);
    
    //We will do the SPI write manually here for speed
    //( the data sheet says it doesn't matter if CSX changes between 
    // data sections but I don't trust it.)
    //CSX low to begin data
    CSX = 0;
    //Write colour to each pixel
    for(int y = 0; y < y2-y1+1 ; y++) {
        for(int x = 0; x < x2-x1+1; x++) {
            spi_write(colour_high);
            spi_write(colour_low);
        }
    }
    //Return CSX to high
    CSX = 1;
}

/*
 * Sets the X,Y position for following commands on the display.
 * Should only be called within a function that draws something
 * to the display.
 */
void set_draw_window(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2) {
    //SEt the column to write to
    lcd_write_command(ST7735_CASET);
    lcd_write_data(0x00);
    lcd_write_data(x1);
    lcd_write_data(0x00);
    lcd_write_data(x2);
    
    //Set the row range to write to
    lcd_write_command(ST7735_RASET);
    lcd_write_data(0x00);
    lcd_write_data(y1);
    lcd_write_data(0x00);
    lcd_write_data(y2);
    
    //Write to RAM
    lcd_write_command(ST7735_RAMWR);
}

/*
 * Draws a single char to the screen.
 * Called by the various string writing functions like print().
 */
void draw_char(unsigned char x, unsigned char y, unsigned char c, unsigned int colour, unsigned char size){
    char i, j;
  
    for(i=0; i<5; i++ ) {
      unsigned char line;
      if(c < 'S')
          line = Font[(c - 32)*5 + i];
      else
          line = Font2[(c - 'S')*5 + i];
      for(j=0; j<7; j++, line >>= 1) {
          if(line & 0x01) {
            if(size == 1)
                //If we are just doing the smallest size font then do a single pixel each
                draw_pixel(x+i, y+j, colour);
            else
                //Otherwise do a small box to represent each pixel
                fill_rectangle(x+(i*size), y+(j*size), x+(i*size)+size, y+(j*size)+size, colour);
            }
        }
    }
}

/*
 * Writes a string to the display as an array of chars at position x, y with 
 * a given colour and size.
 */
void draw_string(unsigned char x, unsigned char y, unsigned int colour, unsigned char size, char *str) {
    //Work out the size of each character
    int char_width = size * 6;
    //Iterate through each character in the string
    int counter = 0;
    while(str[counter] != '\0') {
        //Calculate character position
        int char_pos = x + (counter * char_width);
        //Write char to the display
        draw_char(char_pos, y, str[counter], colour, size);
        //Next character
        counter++;
    }
}

/*
 * Draw a line between two points, using the desired colour. Doesn't do any
 * fancy aliasing or anything.
 * 
 * NOTE: Using floats takes up a huge amount of program memory. I would 
 * recommend commenting out this function unless absolutely necessary.
 * Use fill_rectangle() to draw horizontal and vertical lines instead.
 * 
 * TODO: Also this function doesn't actually work properly.
 */
/*
void draw_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int colour) {
    //Calculate the horizontal length of the line
    int length = x2 - x1;
    //And calculate the rate of change of the graph
    float gradient = (y2 - y1) / length;
    
    //For each horizontal pixel, move down (or up) by the gradient value
    for(int i = 0; i <= length; i++) {
        draw_pixel(x1+i, y1 + (gradient * i), colour);
    }
}
*/