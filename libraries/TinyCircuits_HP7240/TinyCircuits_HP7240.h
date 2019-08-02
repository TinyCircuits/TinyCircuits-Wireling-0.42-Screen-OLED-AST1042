//*******************************************************************
// TinyCircuits - HP7240 OLED Display Library
// Written by: Hunter Hykes for TinyCircuits
//
// Initiated: Wed. 07/31/2019 @ 04:39PM
//   Updated: Wed. 07/31/2019 @ 04:39PM
//
// Revision 0 - Original Release
//	RELEASED: MM/DD/YYYY ***Not Yet Released***
//
//*******************************************************************

#ifndef _TinyCircuits_HP7240_H_
#define _TinyCircuits_HP7240_H_

#include <Arduino.h>
#include <Wire.h>

#define HP7240_DEFAULT_ADDRESS 0x3C
#define HP7240_SETCONTRAST 0x81
#define HP7240_DISPLAYALLON_RESUME 0xA4
#define HP7240_DISPLAYALLON 0xA5
#define HP7240_NORMALDISPLAY 0xA6
#define HP7240_INVERTDISPLAY 0xA7
#define HP7240_DISPLAYOFF 0xAE
#define HP7240_DISPLAYON 0xAF
#define HP7240_SETDISPLAYOFFSET 0xD3
#define HP7240_SETCOMPINS 0xDA
#define HP7240_SETVCOMDETECT 0xDB
#define HP7240_SETDISPLAYCLOCKDIV 0xD5
#define HP7240_SETPRECHARGE 0xD9
#define HP7240_SETMULTIPLEX 0xA8
#define HP7240_SETLOWCOLUMN 0x00
#define HP7240_SETHIGHCOLUMN 0x10
#define HP7240_SETSTARTLINE 0x40
#define HP7240_MEMORYMODE 0x20
#define HP7240_COLUMNADDR 0x21
#define HP7240_PAGEADDR   0x22
#define HP7240_COMSCANINC 0xC0
#define HP7240_COMSCANDEC 0xC8
#define HP7240_SEGREMAP 0xA0
#define HP7240_CHARGEPUMP 0x8D
#define HP7240_SWITCHCAPVCC 0x2
#define HP7240_NOP 0xE3

#define HP7240_WIDTH 72
#define HP7240_HEIGHT 40
#define HP7240_BUFFERSIZE (HP7240_WIDTH*HP7240_HEIGHT)/8

class TinyCircuits_HP7240 {

  public:
    TinyCircuits_HP7240(void); 		//Constructor

    void begin(void);
    void sendCommand(uint8_t); 
    void resetScreen(uint8_t);
    void init(void); 
    void sendFramebuffer(uint8_t *buffer);
    void setCursorX(int x);
    void write(uint8_t *buffer, char c);
    void printSSD(uint8_t *buffer, char *c);

    int xMax = 72;
    int yMax = 40;
  private:
    int _cursorX = 0;
    
};

#endif		//TinyCircuits_HP7240_H_ 
