/*
 * File:   main.c
 * Author: tommy
 *
 * Created on 17 October 2018
 * 
 * Uses a little 8x8 far infrared (thermal infrared) sensor and
 * displays the image on an LCD.
 */

#include <xc.h>
#include "ST7735.h"

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT/T1OSO pin, I/O function on RA7/OSC1/CLKIN/T1OSI)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power Up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
    
//LCD Pins
#define LED RB0
#define CS RB1
#define DAT RB2
#define RES RB3



/*
 * Initialise the SPI interface
 */
void initSPI() {
    
    //TRISB as output if not already
    TRISB = 0x00;
    //TRISC<7> bit must be set
    TRISC = TRISC | 0b10000000;
    //TRISC<4> bit cleared and TRISC<6> bit cleared
    TRISC = TRISC & 0b10101111;
    //TRISA and ~SS bit
    TRISA = 0x00;
    
    //Turn on SPI
    SSPCON = 0b00110000;
    SSPSTAT = 0b00000000;
}

void main(void) {
    //SEt 8MHz Oscillator
    OSCCON = 0b01110001;
    //SET PORTB OUTPUT and turn power LED on
    TRISB = 0x00;
    LED=1; //Boot test LED
    
    //Begin SPI communciation
    initSPI();
    
    //Boot up delay
    delay_ms(500);
    
    //LCD initialisation routine
    initLCD();
    
    //LED off
    LED = 0;
    
    //more delay why not
    delay_ms(500);
    
    
    
    LED = 0;
    
    //Test routine
    while(1) {
        
        fill_rectangle((int)1, (int)1, (int)129, (int)129, 0xFAFA);
        draw_string((int)1, (int)64, 0xAFAF, 3, "Hello");
        draw_string((int)32, (int)98, 0xFF00, 3, "World");
        fill_rectangle((int)0, (int)64, (int)128, (int)65, 0xFFFF);
        fill_rectangle((int)64, (int)0, (int)65, (int)128, 0x0000);

        delay_ms(1000);

        //test_drawing
        fill_rectangle((int)1, (int)1, (int)129, (int)129, 0xAFAF);
        draw_string((int)1, (int)1, 0xFFFF, 2, "Hello World");
        
        delay_ms(1000);
    }
    return;
}

