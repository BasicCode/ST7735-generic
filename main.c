/*
 * File:   main.c
 * Author: tommy
 *
 * Created on 17 October 2018
 * 
 * Uses a little 8x8 far infrared (thermal infrared) sensor and
 * displays the image on an LCD.
 * Written for the PIC 16F913 micro, but should be pretty similar across 
 * the whole PIC 16/18 family.
 */


// PIC18F24J50 Configuration Bit Settings
// 'C' source line config statements
// CONFIG1L
#pragma config PLLDIV = 12      // PLL Prescaler Selection bits (Divide by 12 (48 MHz oscillator input))
// CONFIG1H
#pragma config CPUDIV = OSC1    // CPU System Clock Postscaler (No CPU system clock divide
// CONFIG2L
#pragma config OSC = INTOSCPLL  // Oscillator (INTOSCPLL)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "ST7735.h"


#define LED     RC4 //LED is actually unrelated.


//Some global constants
const unsigned int BG_COLOUR = 0x0000;
const unsigned int colour_list[] = {0xFFAA, 0xF0F0, 0x0F0F, 0xF900, 0x009F, 0x0990, 0xF00F};
const int num_colours = 7;

//IR 'camera' chip receive buffer
unsigned char ir_buffer[64];

/*
 * Initialise the SPI interface
 */
void initSPI() {
    //SET Port Input / Output latches
    TRISB5 = 0;
    TRISB4 = 0;
    TRISC7 = 0;
    TRISC2 = 0;
    TRISC1 = 0;
    TRISC0 = 0;
    
    //SPI initialisation
    SSP1CON1 = 0b00110000;
}

void main(void) {
    
    //Change internal oscillator
    OSCTUNE = 0b0101111;
    OSCCON = 0b01110000; //Set to 8MHz mode
    
    //SET Port Input / Output latches
    TRISC2 = 0; //LED Output
    LED = 1; //Boot test LED
    
    //Begin SPI and I2C communciation
    initSPI();
    
    //Boot up delay
    delay_ms(500);
    
    
    //LCD initialisation routine
    lcd_init();
    
    //LED off
    LED = 0;
    
    //more delay why not
    delay_ms(500);
    
    //Blank out the LCD
    fill_rectangle((int)0, (int)0, (int)128, (int)128, BG_COLOUR);
    
    LED = 1;
    
    int this_colour = 0;
    int box_size = 12;
    
    //Test routine
    while(1) {
        
        for(int x = 1; x < 10; x++) {
            for(int y = 1; y < 10; y++) {
                fill_rectangle((x*box_size), (y*box_size), (x*box_size) + box_size, (y*box_size) + box_size, colour_list[this_colour]);
                
                //cycle colour
                this_colour++;
                if(this_colour == num_colours)
                    this_colour = 0;
            }
        }
    }
    
    while(1); //fail-safe halt point
    return;
}

