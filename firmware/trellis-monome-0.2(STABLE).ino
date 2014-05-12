 /*
RACS 40h translation
May 2014
https://github.com/rbnstubbs/trellis-monome

///////////////////////////////////////////////////////////////////////
This program is free software; you can redistribute it and/or modify 
it under the terms of the GNU General Public License as published by 
the Free Software Foundation; either version 2 of the License, or 
(at your option) any later version.

This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.
///////////////////////////////////////////////////////////////////////

Designed to work with the Adafruit Trellis (four boards)
Uses Trellis XY Library and monomecereal.maxpat, but it will also work with 
monome's serialOSC if the USB chip is flashed. A 3D printed enclosure was 
adapted from Adafruit's implementation. All files are on the git.

This sketch was largely based on the Arduinome clone by Owen Vallis & 
Jordan Hochenbaum: http://flipmu.com/work/arduinome/

http://monome.org/

Robin Stubbs
robin.stubbs@uleth.ca
*/


///Trellis Init
 
    #include <Wire.h>
    #include <Adafruit_Trellis.h>
    #include <Adafruit_Trellis_XY.h>
    
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
    
    int LED = 0;
    
    
void setup() {
      Serial.begin(115200);
      Serial.println("RACS Arduinome");
     
      // INT pin requires a pullup
      pinMode(INTPIN, INPUT);
      digitalWrite(INTPIN, HIGH);
     
      // order is important here!! Start with tile address you want to
      // use as the first one, etc.
      trellis.begin(0x73, 0x72, 0x70, 0x71);
    
////////////////////////////////////////////
/// RACS XY init seq
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
// Global variables 

byte byte0, byte1, row = 0;                      // storage for incoming serial data

boolean WaitingCommand = true;          // true when we expect the next serial byte to be an address value, false otherwise
int command =  0;                       // garbage int to hold address of function

byte x = 0x00;                          // garbage byte to hold x position
byte y = 0x00;                          // garbage byte to hold y position



// The following variables are used to store flags that indicate whether we have received a given message,
// the value of the data in that message.  e.g. IntensityChange = 0 == no intensity change message received,
// IntensityChange = 1 == an intensity change message has been received, and it's value will be in IntensityVal
// The code that eventually acts upon the incoming message will reset the 'Change' variable to 0 once the 
// message has been handled.            

int ShutdownModeChange = 0;            
boolean ShutdownModeVal= false;

int i = 0;                              // temporary variable for looping etc.
int id = 0;                             // temporary storage for incoming message ID


////////////////////////////////////////////////
/// RACS - check button state

    void checkButtons () {                                                /// RACS - WORKING
      delay(8); // 30ms delay is required, dont remove me!

        if (trellis.readSwitches()) {

          for (uint8_t i=0; i<numKeys; i++) {

            if (trellis.justPressed(i)) {
               TrellisToXY(i, TileOffsets, &xVal, &yVal);      
               Serial.write((0 << 4) | (1 & 15));      // OG           Serial.write((0 << 4) | (b & 15));
               Serial.write((yVal << 4) | (xVal & 15));      // OG          Serial.write((id << 4) | (i & 15));              
     }
            if (trellis.justReleased(i)) {
               TrellisToXY(i, TileOffsets, &xVal, &yVal);
               xyTrellisID = XYToTrellis(numKeys, TileOffsets, xVal, yVal);
               Serial.write((0 << 4) | (0 & 15));           /// RACS - THE IMPORTANT SHIT
               Serial.write((yVal << 4) | (xVal & 15));
       	   }
         }
       }
     }
                          


///////////////////////////////////////////////
/// LED Functions

void onLED( byte x, byte y ) {
    byte xMask = 1 << x;
    trellis.setLED(XYToTrellis(numKeys, TileOffsets, y, x));
  }
 
void offLED( byte x, byte y ) {
    byte xMask = (1 << x) ^ 0xff;
    trellis.clrLED(XYToTrellis(numKeys, TileOffsets, y, x));        /// RACS - y and x reversed works here?
   }

void clearRows() {
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
 
void setRows() {
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
  }



///////////////////////////////////////////////
/// RACS Check Serial and do it (straight out of Brian's sketch, which is pretty much straight out of the Arduinome)
/// This should eventually be re-implemented as an interrupt, it seems to lag a little

void checkSerial() {
    byte x, y, state;
    
    do {
      if (Serial.available()) {
        if (WaitingCommand) {          // command is the first byte of the two byte command
          byte0 = Serial.read();
          command = byte0 >> 4;
          WaitingCommand = false;
 
            switch(command) {          // do one byte commands
              
              case 9: // clear command
//                if((byte0 & 1) == 0) clearRows();
//                  else setRows();
//                  WaitingCommand = true; // next byte is a new command
                  break;
            }
           if((command > 9) || (command < 2)) WaitingCommand = true; // command outside range so must be an error
            }
            else {
              WaitingCommand = true; // the next byte is the first byte of the next command
              byte1 = Serial.read(); // read the second byte of this command
 
              switch(command) {
 
                case 2: // LED command
                state = byte0 & 0x0f; // is the command for on or off
                x = ((byte1 >> 4) & 0x07); // mask so we don't go over the 8 by 8 grid
                y = ((byte1 & 15) & 0x07);
 
                  if (state == 0) {
                  offLED(x, y);
                 }
                   else {
                   onLED(x, y);
                   }
                  break;
 
                case 3: // led intensity command
                break;
                 
                case 4: // led test command
//                  if( (byte1 & 1) == 0) {
//                    setRows();
//                 }
//                  else {
//                    clearRows();
//                 }
                break;
                
                case 5: // enable ADC command - but we don't do this
                break;
                
                case 6: // shutdown command - not sure what the monome is expected to do here
                  ShutdownModeChange = true;
                  ShutdownModeVal = byte1 & 15;
                break;
                
                case 7: // led row command
                  y = byte0 & 0x07; // mask this value so we don't write to an invalid address.
                  x = 0;
                  for(byte i = 0x1; i != 0x0; i <<= 1 ) {
                    if( (i & byte1) != 0) {
                    onLED(x, y);
                   }
                  else {
                    offLED(x, y);
                  }
                  x++;
                }
                break;
 
                case 8: // colomn command
                  x = byte0 & 0x07; // mask this value so we don't write to an invalid address.
                  y = 0;
                  for(byte i = 0x1; i != 0x0; i <<= 1 ) {
                    if( (i & byte1) != 0) {
                      onLED(x, y);
                    }
                    else {
                      offLED(x, y);
                    }
                  y++;
                }
                break;
                
                default:
                  onLED(7, 7);
                break;
 
               } // end switch(command)
              } // end of else in if(WaitingCommand)
             } // end of if(Serial.available();
            } // end of do
           while (Serial.available() > 8); // keep on going if we have a lot of commands stacked up
          }


/// RACS - Send it 
void loop() {
  checkButtons();
  checkSerial();
  trellis.writeDisplay();
 }

//eof
