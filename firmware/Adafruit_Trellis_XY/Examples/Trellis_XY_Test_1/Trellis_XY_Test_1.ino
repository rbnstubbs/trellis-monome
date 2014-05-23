/*************************************************** 
  This is a test example for the Adafruit Trellis w/HT16K33 
  Added Adafruit_Trellis_XY library
  Print out X/Y values...
  
  G. Kuetemeyer
  03/20/2014
  gk@pobox.com


  Designed specifically to work with the Adafruit Trellis 
  ----> https://www.adafruit.com/products/1616
  ----> https://www.adafruit.com/products/1611

 ****************************************************/

#include <Wire.h>
#include <Adafruit_Trellis.h>
#include <Adafruit_Trellis_XY.h>

/*************************************************** 
  This example shows reading buttons and setting/clearing buttons in a loop
  Also sending X/Y values to serial port.

  Up to 8 tiles can be used but this example will use 1 tile
 ****************************************************/

Adafruit_Trellis matrix0 = Adafruit_Trellis();

Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0);

#define NUMTRELLIS 1
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
 
  // INT pin requires a pullup
  pinMode(INTPIN, INPUT);
  digitalWrite(INTPIN, HIGH);
  
  // Order is important here!! Start with tile address you want to
  // use as the first one, etc. Note that this will vary depending
  // on how you/ve set up addressing.
  trellis.begin(0x70);

  // light up all the LEDs in order
  for (byte y = 0; y < 4; y++) {
    for (byte x = 0; x < 4; x++) {
      trellis.setLED(trellisXY.getTrellisId(x, y));
      trellis.writeDisplay();    
      delay(50);
    }
  }
  // then turn them off
  for (byte y = 0; y < 4; y++) {
    for (byte x = 0; x < 4; x++) {
      trellis.clrLED(trellisXY.getTrellisId(x, y));
      trellis.writeDisplay();    
      delay(50);
    }
  }
}

//-----------------------------------------------------------
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


