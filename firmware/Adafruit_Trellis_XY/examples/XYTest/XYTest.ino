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

     ****************************************************/

    #define MOMENTARY 0
    #define LATCHING 1
    // set the mode here
    #define MODE LATCHING


    Adafruit_Trellis matrix0 = Adafruit_Trellis();
    Adafruit_Trellis matrix1 = Adafruit_Trellis();
    Adafruit_Trellis matrix2 = Adafruit_Trellis();
    Adafruit_Trellis matrix3 = Adafruit_Trellis();

    Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0, &matrix1, &matrix2, &matrix3);

    #define NUMTRELLIS 4

    #define numKeys (NUMTRELLIS * 16)

    #define INTPIN A2

    //-----------------------------------------------------------
    // define offsets for 4X4 Tiles
    struct TileOffsets TileOffsets = {
      { 0, 4, 0, 4, 0, 0, 0, 0 },
      { 0, 0, 4, 4, 0 ,0, 0, 0 }
    };

    uint8_t xVal;
    uint8_t yVal;
    uint8_t xyTrellisID;

    //-----------------------------------------------------------
    void setup() {
      Serial.begin(9600);
      Serial.println("Trellis Demo");
     
      // INT pin requires a pullup
      pinMode(INTPIN, INPUT);
      digitalWrite(INTPIN, HIGH);
     
      // order is important here!! Start with tile address you want to
      // use as the first one, etc.
      trellis.begin(0x73, 0x72, 0x70, 0x71);

////////////////////////////////////////////
// RACS XY init seq
      for (uint8_t yVal=0; yVal<8; yVal++) {
        xVal=0;
        while (xVal<8) {
        int LED = XYToTrellis(numKeys, TileOffsets, xVal, yVal);
        trellis.setLED(LED);
        trellis.writeDisplay();
        xVal++;
       delay(20);
      }
         } 
        for (uint8_t xVal=0; xVal<8; xVal++) {
        yVal=0;
        while (yVal<8) {
        int LED = XYToTrellis(numKeys, TileOffsets, xVal, yVal);
        trellis.clrLED(LED);
        trellis.writeDisplay();
        yVal++;
       delay(20);
        }
       }
    }

     
///////////////////////////////////////////////////////////////////////
    void loop() {
      delay(30); // 30ms delay is required, dont remove me!
     
      if (MODE == LATCHING) {
        // If a button was just pressed or released...
        if (trellis.readSwitches()) {
          // go through every button
          for (uint8_t i=0; i<numKeys; i++) {
            // if it was pressed...
       if (trellis.justPressed(i)) {
              // print out X/Y values
              Serial.println("----------");
              Serial.print("v"); Serial.println(i);
              TrellisToXY(i, TileOffsets, &xVal, &yVal);
              Serial.print("x: "); Serial.println(xVal, DEC);
              Serial.print("y: "); Serial.println(yVal, DEC);
              // also print out Trellis ID #
              xyTrellisID = XYToTrellis(numKeys, TileOffsets, xVal, yVal);
              Serial.print("xyTrellisID: "); Serial.println(xyTrellisID, DEC);
         // Alternate the LED
         if (trellis.isLED(i)) {
           trellis.clrLED(i);
              }
         else {
           trellis.setLED(i);
              }
            }
          }
          // tell the trellis to set the LEDs we requested
          trellis.writeDisplay();
        }
      }




/// RACS EDITS (adds momentary)

if (MODE == MOMENTARY) {
    // If a button was just pressed or released...
    if (trellis.readSwitches()) {
      // go through every button
      for (uint8_t i=0; i<numKeys; i++) {
	// if it was pressed, turn it on
	if (trellis.justPressed(i)) {
    //XY TOO
      Serial.println("----------");
	  Serial.print("v"); Serial.println(i);
      TrellisToXY(i, TileOffsets, &xVal, &yVal);
      Serial.print("x: "); Serial.println(xVal, DEC);
      Serial.print("y: "); Serial.println(yVal, DEC);
	  trellis.setLED(i);
      // also print out Trellis ID #
              xyTrellisID = XYToTrellis(numKeys, TileOffsets, xVal, yVal);
              Serial.print("xyTrellisID: "); Serial.println(xyTrellisID, DEC);
	} 
	// if it was released, turn it off
	if (trellis.justReleased(i)) {
      Serial.println("----------");
	  Serial.print("^"); Serial.println(i);
      TrellisToXY(i, TileOffsets, &xVal, &yVal);
      Serial.print("x: "); Serial.println(xVal, DEC);
      Serial.print("y: "); Serial.println(yVal, DEC);
    // also print out Trellis ID #
              xyTrellisID = XYToTrellis(numKeys, TileOffsets, xVal, yVal);
              Serial.print("xyTrellisID: "); Serial.println(xyTrellisID, DEC);
	  trellis.clrLED(i);
	}
      }
      // tell the trellis to set the LEDs we requested
      trellis.writeDisplay();
    }
  }
  
}