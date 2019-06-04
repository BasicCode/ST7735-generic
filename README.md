# ST7735 128x128 px LCD Driver Library

This is a very basic, bare-bones, library for the ST7735 LCD display driver, it is designed to be as lean as possible
for PIC micros with small program memory. It provides a basic initialisation function,
SPI data flow control, optional software SPI (not recommended), text, and rectangle drawing. More advanced graphics 
functions have been omitted to save program space.<br>
Although it is originally designed for a PIC 16F913, the code should be fairly transferrable to other systems after
updating the pins in the header file.

## Usage
Download the project and include the **ST7735.h** file in your project. Modify the pin definitions in the header file 
to reflect your set up. More detail on pin definitions is in the ST7735 datasheet.<br>
Hardware SPI initialisation will need to be done seperately depending on your device, alternately there is a software SPI
function included. The software SPI runs very slowly though (I clocked it at 40kHz), and its almost un-usable for a
colour LCD display.<br>
```
initLCD();
```
You should see the display turn from white / grey to a random speckled pattern. You are now ready to start drawing.
Typically you would begin by blanking the screen with a black rectangle:
```
fill_rectangle(0, 0, 128, 128, 0x0000);
```

+ ST7735 Datasheet (https://www.displayfuture.com/Display/datasheet/controller/ST7735.pdf)
+ A brief example is included in the **main.c** file.

## Notes
+ Allow a generous delay after your SPI initialisation routine, and then initialise the LCD.
+ You will need to set SPIBUF and SPIIDLE in the header file to your own device's SPI Tx buffer and SPI busy flag respectively (it changes for each device and each model).
+ You will also need to write your own SPI initialisation routine because it is different for each chip.
+ The maximum speed that I have got out of these chips is an SPI clock of about 8MHz.