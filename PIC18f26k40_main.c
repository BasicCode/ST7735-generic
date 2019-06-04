/*
 * File:   main.c
 * Author: tommy
 *
 * Created on 17 October 2018
 * Updates on 4th June 2019
 * 
 * Uses a little 8x8 far infrared (thermal infrared) sensor and
 * displays the image on an LCD.
 * Written for the PIC 18F26K42 micro. The SPI routine and individual registers
 * differ across devices so you will need to implement them manually.
 */


 // PIC18F26K42 Configuration Bit Settings

 // 'C' source line config statements

 // CONFIG1L
#pragma config FEXTOSC = ECH    // External Oscillator Selection (EC (external clock) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = HFINTOSC_64MHZ// Reset Oscillator Selection (HFINTOSC with HFFRQ = 64 MHz and CDIV = 1:1)

// CONFIG1H
#pragma config CLKOUTEN = OFF   // Clock out Enable bit (CLKOUT function is disabled)
#pragma config PR1WAY = ON      // PRLOCKED One-Way Set Enable bit (PRLOCK bit can be cleared and set only once)
#pragma config CSWEN = ON       // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON       // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor enabled)

// CONFIG2L
#pragma config MCLRE = EXTMCLR  // MCLR Enable bit (If LVP = 0, MCLR pin is MCLR; If LVP = 1, RE3 pin function is MCLR )
#pragma config PWRTS = PWRT_OFF // Power-up timer selection bits (PWRT is disabled)
#pragma config MVECEN = OFF      // Disable vectored interrupts
#pragma config IVT1WAY = ON     // IVTLOCK bit One-way set enable bit (IVTLOCK bit can be cleared and set only once)
#pragma config LPBOREN = OFF    // Low Power BOR Enable bit (ULPBOR disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable bits (Brown-out Reset disabled)

// CONFIG2H
#pragma config BORV = VBOR_2P45 // Brown-out Reset Voltage Selection bits (Brown-out Reset Voltage (VBOR) set to 2.45V)
#pragma config ZCD = OFF        // ZCD Disable bit (ZCD disabled. ZCD can be enabled by setting the ZCDSEN bit of ZCDCON)
#pragma config PPS1WAY = ON     // PPSLOCK bit One-Way Set Enable bit (PPSLOCK bit can be cleared and set only once; PPS registers remain locked after one clear/set cycle)
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config DEBUG = OFF      // Debugger Enable bit (Background debugger disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Extended Instruction Set and Indexed Addressing Mode disabled)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period selection bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

// CONFIG3H
#pragma config WDTCWS = WDTCWS_7// WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC      // WDT input clock selector (Software Control)

// CONFIG4L
#pragma config BBSIZE = BBSIZE_512// Boot Block Size selection bits (Boot Block size is 512 words)
#pragma config BBEN = OFF       // Boot Block enable bit (Boot block disabled)
#pragma config SAFEN = OFF      // Storage Area Flash enable bit (SAF disabled)
#pragma config WRTAPP = OFF     // Application Block write protection bit (Application Block not write protected)

// CONFIG4H
#pragma config WRTB = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-30000Bh) not write-protected)
#pragma config WRTC = OFF       // Boot Block Write Protection bit (Boot Block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)
#pragma config WRTSAF = OFF     // SAF Write protection bit (SAF not Write Protected)
#pragma config LVP = ON         // Low Voltage Programming Enable bit (Low voltage programming enabled. MCLR/VPP pin function is MCLR. MCLRE configuration bit is ignored)

// CONFIG5L
#pragma config CP = OFF         // PFM and Data EEPROM Code Protection bit (PFM and Data EEPROM code protection disabled)

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

#include <xc.h>
#include "ST7735.h"


#define LED     RC4 //LED is actually unrelated.


//Some global constants
const unsigned int BG_COLOUR = 0x0000;
const unsigned int colour_list[] = {0xFFAA, 0xF0F0, 0x0F0F, 0xF900, 0x009F, 0x0990, 0xF00F};
const int num_colours = 7;

/*
 * Initialise the SPI interface
 */
void initSPI() {
	//Set PPS ports for the SPI1 module
	RC3PPS = 0b11111;
	RC4PPS = 0b11110;
	//Set SPI baud rate FOSC / 4
	SPI1BAUD = 0x01;
	//Transmit only mode
	SPI1CON2 = 0b00000010;
	//Set the SPI bus to resting positive clock
	SPI1CON1 = 0b00100000;
	//Enable SPI bus
	SPI1CON0 = 0b10000011;
}

void main(void) {
    
    //Change internal oscillator
    //Set oscillator to 32MHz
	OSCFRQ = 0b00000110;
    
    //SET Port Input / Output latches
    TRISC = 0x00; //LED Output
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

