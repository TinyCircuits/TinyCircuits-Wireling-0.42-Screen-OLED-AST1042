/************************************************************************
 * HP7240 Wireling 0.42" OLED Display Demo/Test Code -
 * This code will display text and a Sprite at the same time when compiled. 
 *
 * Hardware by: TinyCircuits
 * Written by: Ben Rose, Laverena Wienclaw, & Hunter Hykes for TinyCircuits
 *
 * Initiated: Mon. 11/20/2017 
 * Updated: Tue. 08/02/2019
 ************************************************************************/

#include <Wire.h>                   // For I2C communication
#include <TinyScreen.h>             // For interfacing with TinyScreen+
#include "Font_042.h"               // The font displayed on the screen
#include "TinyCircuits_HP7240.h"    // Library for OLED screen
#include "exampleSprites.h"         // Holds arrays of example Sprites

#define SCREEN_PORT 0               // Port used by the 0.42 OLED Wireling
#define RESET_PIN (uint8_t)A0       // A0 corresponds to Port 0. Update if changing ports.
                                    // Port1: A1, Port2: A2, Port3: A3
// Variable for the 0.42" Wireling screen
TinyCircuits_HP7240 TiniestScreen;

#define xMax TiniestScreen.xMax
#define yMax TiniestScreen.yMax

TinyScreen display = TinyScreen(TinyScreenPlus);

// This is the data structure for a TinyScreen Sprite
typedef struct {
  int x;             // X coordinate (top left corner) of where Sprite is placed on screen initially 
  int y;             // Y coordinate (top left corner) of where Sprite is placed on screen initially 
  int width;         // Wideness of Sprite
  int height;        // Height of Sprite
  int bitmapNum;     // Index of animation stage of Sprite (only used when a Sprite is animated)
  const unsigned int * bitmap; // The array of 0's and 1's that determines appearance of Sprite
} Sprite;

Sprite bunnySprite = {0, 0, 13, 13, 0, bunnyBitmap};
Sprite duckSprite = {59, 3, 13, 10, 0, duckBitmap};
int amtSprites = 2; 
Sprite * spriteList[2] = {
  &bunnySprite, &duckSprite
};

uint8_t oledbuf[HP7240_BUFFERSIZE]; // Buffer to hold screen data

int bufIndex = 0; // Buffer index in respect to the array of pixels on the screen
int textPos = 0;  // Used to make text move left or right

int bunnySpeed = 2;
int bleh = 0;

void setup() {
  SerialUSB.begin(9600);
  pinMode(4, OUTPUT);
  digitalWrite(4, HIGH); // power Wireling Adapter TinyShield
  delay(10);

  TiniestScreen.begin();    // begin I2C communications with screen
  selectPort(SCREEN_PORT);  // This port# matches the one labeled on the adapter board
  TiniestScreen.resetScreen(RESET_PIN);   // resets Wireling screen MUST BE CALLED BEFORE init()
  TiniestScreen.init();     // initialize screen
  clearOLED();              // Clear Display Buffer, isn't fully cleared here, frame buffer must be sent
  delay(2);
  TiniestScreen.sendFramebuffer(oledbuf); // Send Cleared Buffer

  // Initialize appearance of TinyScreen
  display.begin();
  display.setFlip(true);
  display.setBrightness(15);
  display.setFont(thinPixel7_10ptFontInfo);
  display.fontColor(TS_8b_White, TS_8b_Black); // white text, black background
  display.setCursor(0, 0);
  display.print("0.42 OLED Test");
  
}

// Clear the screen, put new text on the screen -> clear to create scrolling effect 
void loop() {
  clearOLED();
  drawSprites();
  
  textPos = 3*72;
  TiniestScreen.setCursorX(textPos);
  
  TiniestScreen.printSSD(oledbuf, "Bun and Duck"); // text must be written after sprites
  
  TiniestScreen.sendFramebuffer(oledbuf);
}

void selectPort(int port) {
  Wire.beginTransmission(0x70);
  Wire.write(0x04 + port);
  Wire.endTransmission(0x70);
}

/* The OLED screen is 72 by 40 pixels. The screen uses x and y coordinates,
   but with the twist that the 0,0 point is at the top left:

   **************************************************************************
   |  (0,0), (1,0), (...)                                            (71, 0)|
   |  (0,1),                                                                |
   |  (...)                                                                 |
   |                                                                        |
   |                                                                        |
   |                                                                        |
   |  (0,39)                                                         (71,39)|
   **************************************************************************
                              (Not to scale)
  This function draws the Sprite by iterating through y 'slices' to find the x coord
  at which the Sprite starts as defined by the data structure Sprite.
  Each x pixel in these y slices is then flipped on or off using the bool pixelOn

  Fun fact: The x coord must be checked for being on the screen (within 72) due to
  the way the OLED screen works (writes all the information for the top quad of the
  screen before starting the next quad information). That's why there seems to be
  a redundant check for the x coordinate being within position 72.
*/

void drawSprites() {
  clearOLED();

  for (int y = 0; y < yMax; y++) { // Iterates through all y coordinates of the screen
    for (int spriteIndex = 0; spriteIndex < amtSprites; spriteIndex++) { // Iterates pixels for each Sprite

      // Sets currentSprite to the Current Sprite being accessed in the array
      Sprite *currentSprite = spriteList[spriteIndex]; 
      
      // Checks if the current Y-coordinate is within the current sprite's area
      if (y >= currentSprite->y && (y < (currentSprite->y + currentSprite->height))) { 
        int endX = currentSprite->x + currentSprite->width;
        if (currentSprite->x < xMax && endX > 0) { //checks if the sprite is within the X-coordinate bounds of the screen
          int xBitmapOffset = 0;
          int xStart = 0;
          if (currentSprite->x < 0) {
            xBitmapOffset -= currentSprite->x;
          }
          if (currentSprite->x > 0) {
            xStart = currentSprite->x;
          }
          int yBitmapOffset = (y - currentSprite->y) * currentSprite->width;

          for (int x = xStart; x < endX && x < xMax; x++) { //iterates through the X-bounds of the sprite
            bool pixelOn = currentSprite->bitmap[xBitmapOffset + yBitmapOffset++];
            if (pixelOn) {
              setPixel(x, y); //sets the pixel if a 1 is found in the bitmap
            }
          }
        }
      }
    }
  }
}

// Writes blank data to OLED to clear data
void clearOLED() {
  for (int i = 0; i < HP7240_BUFFERSIZE; i++) {
    oledbuf[i] = 0x00;
  }
}

void setPixel(int px, int py) {
  int pos = px;       //holds the given X-coordinate
  if (py > 7) { // if Y > 7 (the number of indices in a byte)
    pos += (py/8)*xMax; // bump down to the next row by increasing X by the screen width by the number of necessary rows
  }
  py = (py % 8);  // adjusts Y such that it can be written within the 0-7 bounds of a byte
  oledbuf[pos] |= (1 << (py)); // the bits of the byte within the buffer are set accordingly by placing a 1 in the respective bit location of the byte
}
