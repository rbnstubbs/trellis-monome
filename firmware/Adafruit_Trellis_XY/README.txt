Adafruit_Trellis_XY

Copyright (C) 2014  George Kuetemeyer (gk@pobox.com)
   
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
   
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
  GNU General Public License for more details.
   
You should have received a copy of the GNU General Public License
along with this program. If not, see <http://www.gnu.org/licenses/>.


This is a library for the Adafruit Trellis PCB

Designed specifically to work with the Adafruit Trellis!
  ----> https://www.adafruit.com/products/1611
  ----> https://www.adafruit.com/products/1616
   
Version 1.2

---------------------------------------------------------------------------------
DESCRIPTION:

Adafruit_Trellis_XY is a library that can map one or more
Adafruit Trellises into a set of X/Y coordinates.

---------------------------------------------------------------------------------
INSTALLATION:

- Press "Download ZIP" button on GitHub page.
- Download Zip file and unzip it.
- Rename "Adafruit_Trellis_XY-master" folder name to "Adafruit_Trellis_XY"
- Copy the "Adafruit_Trellis_XY folder to your "libraries" directory.

In the Arduino IDE, create a new sketch and select fromn the menu bar
"Sketch->import Library->Adafruit_Trellis_XY. Be sure to place
#include <Adafruit_Trellis_XY.h> at the top of your sketch along with
other required libraries ( <Adafruit_Trellis.h>, etc.).

---------------------------------------------------------------------------------
CREATION:

Adafruit_Trellis_XY trellisXY = Adafruit_Trellis_XY();

---------------------------------------------------------------------------------
METHODS:

void begin (byte number_of_keys) // initialize trellisXY with total number of keys

void setOffsets (byte tile_number, byte x_offset, byte y_offset) // set X/Y offsets

void getTrellisX (byte trellisId) // get X coordinate for given trellis ID

void getTrellisY (byte trellisId) // get Y coordinate for given trellis ID

byte getTrellisId (byte xIn, byte yIn) // get Trellis ID for given X/Y coordinates
   
---------------------------------------------------------------------------------
MAPPING TRELLIS TILE XY OFFSETS
   
In order for this library to function correctly, it's important that you
work out required X and Y offsets for each tile. Here is an example using
four tiles in a square (8X8 buttons/leds):
   
  -----
  |2|3|  <---- tile layout for 8X8 matrix
  |0|1|  
  -----
   
Given that layout, this code will set up things correctly, assuming you've
defined an object trellisXY:
   
trellisXY.setOffsets(0, 0, 0);
trellisXY.setOffsets(1, 4, 0);
trellisXY.setOffsets(2, 0, 4);
trellisXY.setOffsets(3, 4, 4);

Be sure to reflect this layout when you set up trellis.begin. In the case of
a 4X4 matrix you will want to arrange your tile addresses to match this layout.

  trellis.begin(0x70, 0x71, 0x72, 0x73);

If you haven't physically arranged the tiles that way, then rearrange the entries
in the begin statment. Here is how I had to address my 4X4 matrix:

  trellis.begin(0x72, 0x73, 0x70, 0x71);
   
Here's another example:
   
  ---------
  |0|1|2|3|  <------ tile layout for 4X8 matrix
  ---------
   
  
trellisXY.setOffsets(0, 0, 0);
trellisXY.setOffsets(1, 4, 0);
trellisXY.setOffsets(2, 8, 0);
trellisXY.setOffsets(3, 12, 0);
   
Here is simplest example for one Trellis
   
  ---
  |0|  <------ tile layout for single Trellis
  ---
   
trellisXY.setOffsets(0, 0);

---------------------------------------------------------------------------------
SAMPLE CODE: 

//------------------------------------------------------------------------
#include <Wire.h>
#include <Adafruit_Trellis.h>
#include <Adafruit_Trellis_XY.h>

//------------------------------------------------------------------------
  This example shows reading buttons and setting/clearing buttons in a loop
  Also sending X/Y values to serial port.

  Up to 8 tiles can be used. This example uses 4 tiles

//------------------------------------------------------------------------
Adafruit_Trellis matrix0 = Adafruit_Trellis();
Adafruit_Trellis matrix1 = Adafruit_Trellis();
Adafruit_Trellis matrix2 = Adafruit_Trellis();
Adafruit_Trellis matrix3 = Adafruit_Trellis();

Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0, &matrix1, &matrix2, &matrix3);

#define NUMTRELLIS 4
#define NKEYS (NUMTRELLIS * 16)

Adafruit_Trellis_XY trellisXY = Adafruit_Trellis_XY();

#define INTPIN A2

//-----------------------------------------------------------
void setup() {
  Serial.begin(9600);
  Serial.println("Trellis X/Y Demo");

  // define number of keys
  trellisXY.begin(NKEYS);
  // set up x/y offsets
  trellisXY.setOffsets(0, 0, 0);
  trellisXY.setOffsets(1, 4, 0);
  trellisXY.setOffsets(2, 0, 4);
  trellisXY.setOffsets(3, 4, 4);
  
  // INT pin requires a pullup
  pinMode(INTPIN, INPUT);
  digitalWrite(INTPIN, HIGH);
  
  // Order is important here!! Start with tile address you want to
  // use as the first one, etc. Note that this will vary depending
  // on how you/ve set up addressing.
  trellis.begin(0x72, 0x73, 0x70, 0x71);

  // light LEDs 
  for (byte y = 0; y < 8; y++) {
    for (byte x = 0; x < 8; x++) {
      trellis.setLED(trellisXY.getTrellisId(x, y));
      trellis.writeDisplay();    
      delay(50);
    }
  }
  // then turn them off
  for (byte y = 0; y < 8; y++) {
    for (byte x = 0; x < 8; x++) {
      trellis.clrLED(trellisXY.getTrellisId(x, y));
      trellis.writeDisplay();    
      delay(50);
    }
  }
}

//------------------------------------------------------------------------
void loop() {
  delay(30);  
  // If a button was just pressed or released...
  if (trellis.readSwitches()) {
    // go through every button
    for (byte i=0; i < trellisXY.numKeys; i++) {
      byte xValue;
      byte yValue;
      byte xyTrellisID;
      // if it was pressed...
      if (trellis.justPressed(i)) {
        // print out X/Y values
        Serial.println("----------");
        // get x/y values
        xValue = trellisXY.getTrellisX(i);
        yValue = trellisXY.getTrellisY(i);
        Serial.print("x: "); Serial.println(xValue, DEC);
        Serial.print("y: "); Serial.println(yValue, DEC);
        // get Trellis ID from x/y values
        xyTrellisID = trellisXY.getTrellisId(xValue, yValue);
        Serial.print("xyTrellisID: "); Serial.println(xyTrellisID, DEC);
	// Alternate the LED
	if (trellis.isLED(i)) 
	  trellis.clrLED(i);
	else
	  trellis.setLED(i);
        }
     } 
  }
  trellis.writeDisplay();
}

