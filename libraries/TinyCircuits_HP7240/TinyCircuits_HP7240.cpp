//*******************************************************************
// TinyCircuits - HP7240 OLED Display Library
// Written by: Hunter Hykes and Ben Rose for TinyCircuits
//
// 08/02/2019
//
// Revision 0 - Original Release
//
//*******************************************************************

#include <stdlib.h>
#include <Arduino.h>
#include <Wire.h>
#include "TinyCircuits_HP7240.h"
#include "Font_042.h"

//Constructor
TinyCircuits_HP7240::TinyCircuits_HP7240(void) {
  //Nothing
}

//Start the I2C Communication
void TinyCircuits_HP7240::begin(void) {
  Wire.begin();
}

//Send Command Function 
void TinyCircuits_HP7240::sendCommand(uint8_t command) {
  Wire.beginTransmission(HP7240_DEFAULT_ADDRESS);
  Wire.write(0x00);
  Wire.write(command);
  Wire.endTransmission();
  delay(5);
}

void TinyCircuits_HP7240::resetScreen(uint8_t RESET_PIN) {
  // cycle screen reset as specified in datasheet
  pinMode(RESET_PIN, OUTPUT);
  digitalWrite(RESET_PIN, HIGH);
  delay(100);
  digitalWrite(RESET_PIN, LOW);
  delay(100);
  digitalWrite(RESET_PIN, HIGH);
  delay(100);
}

//Screen Initialization
void TinyCircuits_HP7240::init(void) {
  // before calling this function, perform the following on the reset pin (RST):
  // RST = 1; delay(100); RST = 0; delay(100); RST = 1; delay(100);
  
  // Turn display off
  sendCommand(HP7240_DISPLAYOFF);

  sendCommand(HP7240_SETDISPLAYCLOCKDIV);
  sendCommand(0x80);

  sendCommand(HP7240_SETMULTIPLEX);
  sendCommand(40 - 1);

  sendCommand(HP7240_SETDISPLAYOFFSET);
  sendCommand(0x00);

  sendCommand(HP7240_SETSTARTLINE | 0x00);


  //sendCommand(HP7240_SEGREMAP | 0x1);
  //sendCommand(HP7240_COMSCANDEC);
  //flip
  sendCommand(HP7240_SEGREMAP);
  sendCommand(HP7240_COMSCANINC);

  sendCommand(HP7240_SETCOMPINS);
  sendCommand(0x12);

  // Max contrast
  sendCommand(HP7240_SETCONTRAST);
  sendCommand(0xAF);

  sendCommand(HP7240_SETPRECHARGE);
  sendCommand(0xF1);

  sendCommand(HP7240_SETVCOMDETECT);
  sendCommand(0x40);

  sendCommand(HP7240_DISPLAYALLON_RESUME);

  // Non-inverted display
  sendCommand(HP7240_NORMALDISPLAY);

  // We use internal charge pump
  sendCommand(HP7240_CHARGEPUMP);
  sendCommand(0x14);

  // Turn display back on
  sendCommand(HP7240_DISPLAYON);


  // Horizontal memory mode
  sendCommand(HP7240_MEMORYMODE);
  sendCommand(0x00);
}

void TinyCircuits_HP7240::sendFramebuffer(uint8_t *buffer) {
  sendCommand(HP7240_COLUMNADDR);
  sendCommand(28 + 0x00);
  sendCommand(28 + 72 - 1);

  sendCommand(HP7240_PAGEADDR);
  sendCommand(0x00);
  sendCommand(0x05);

  // We have to send the buffer as 16 bytes packets
  // Our buffer is 1024 bytes long, 1024/16 = 64
  // We have to send 64 packets
  for (uint8_t packet = 0; packet < 45; packet++) {
    Wire.beginTransmission(HP7240_DEFAULT_ADDRESS);
    Wire.write(0x40);
    for (uint8_t packet_byte = 0; packet_byte < 8; ++packet_byte) {
      Wire.write(buffer[packet * 8 + packet_byte]);
    }
    Wire.endTransmission();
  }
}

void TinyCircuits_HP7240::setCursorX(int x) {
  _cursorX = x;
}

void TinyCircuits_HP7240::write(uint8_t *buffer, char c) {
  if (c < ' ' || c > '}+2')return; // if character is invalid
  c -= ' '; // adjust to proper index of character array

  for (int i = 0; i < 5; i++) {
    buffer[_cursorX++] = Font5x7[c * 5 + i];
    if (_cursorX > HP7240_BUFFERSIZE)_cursorX = 0;
  }
  buffer[_cursorX++] = 0;
  if (_cursorX > HP7240_BUFFERSIZE)_cursorX = 0;
  return;

  for (int i = 0; i < 5; i++) {
    uint8_t h = Font5x7[c * 5 + i]; // 5 is from character width, c is the character and also array index, i is each "column" of the character
    if (h) {
      uint8_t l = ((h & (1 << 3)) ? (3 << 6) : 0) | ((h & (1 << 2)) ? (3 << 4) : 0) | ((h & (1 << 1)) ? (3 << 2) : 0) | ((h & (1 << 0)) ? (3 << 0) : 0);
      h = ((h & (1 << 7)) ? (3 << 6) : 0) | ((h & (1 << 6)) ? (3 << 4) : 0) | ((h & (1 << 5)) ? (3 << 2) : 0) | ((h & (1 << 4)) ? (3 << 0) : 0);
      buffer[_cursorX] = l;
      buffer[xMax + _cursorX++] = h;
      if (_cursorX > xMax)_cursorX = 0;
      buffer[_cursorX] = l;
      buffer[xMax + _cursorX++] = h;
      if (_cursorX > xMax)_cursorX = 0;
    }
  }
  buffer[_cursorX] = 0;
  buffer[xMax + _cursorX++] = 0;
  if (_cursorX > 71)_cursorX = 0;
}

void TinyCircuits_HP7240::printSSD(uint8_t *buffer, char * s) {
  while (*s) {
    write(buffer, *s);
    s++;
  }
}
