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
Uses Trellis XY Library by George Keutemeyer 
( https://github.com/Gkuetemeyer/Adafruit_Trellis_XY ) and an updated version 
of monomecereal.maxpat by James Drake ( http://monome.org/docs/doku.php?id=app:monomecereal ), 
but it will also work with monome's serialOSC if the USB chip is flashed, or
serialPyio, etc. A 3D printed enclosure was adapted from Adafruit's 
implementation. All files are on the git.

This sketch was largely based on the Arduinome clone by Owen Vallis & 
Jordan Hochenbaum: http://flipmu.com/work/arduinome/

http://monome.org/

Robin Stubbs
robin.stubbs@uleth.ca
*/


///Trellis and XY Init
 
    #include <Wire.h>
    #include <Adafruit_Trellis.h>
    #include <Adafruit_Trellis_XY.h>
    
    Adafruit_Trellis matrix0 = Adafruit_Trellis();
    Adafruit_Trellis matrix1 = Adafruit_Trellis();
    Adafruit_Trellis matrix2 = Adafruit_Trellis();
    Adafruit_Trellis matrix3 = Adafruit_Trellis();

    Adafruit_TrellisSet trellis =  Adafruit_TrellisSet(&matrix0, &matrix1, &matrix2, &matrix3);

    #define NUMTRELLIS 4
    #define NKEYS (NUMTRELLIS * 16)

    Adafruit_Trellis_XY trellisXY = Adafruit_Trellis_XY();

    #define INTPIN A3
        

    /// SETUP    
void setup() {
      Serial.begin(115200);
//      Serial.println("racs arduinome");
      
      // XY Init:
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
     
      // order is important here!! Start with tile address you want to
      // use as the first one, etc.
      trellis.begin(0x71, 0x70, 0x72, 0x73);

  /// XY init seq SET
   setRows();
 
  /// INTERRUPT INIT - NOTE: works with ATMega 128 based boards ONLY, 
  // can be adapted for others
  // Set up 8-bit counter 2, output compare switched off,
  // normal waveform generation (whatever that might mean)
//  TCCR2A = 0;
  // set counter to be clocked at 16Mhz/8 = 2Mhz
//  TCCR2B = 1<<CS21;

  // set the interrupt mask so that we get an interrupt
  // on timer 2 overflow, i.e. after 256 clock cycles.
  // The timer 2 interrupt routine will execute every
  // 128 uS.
//  TIMSK2 = 1<<TOIE2;

  // NOTE: In my efforts to try to get this
  // code to work reliably at 115200 baud
  // I went about disabling unused timers that
  // are set up by the Arduino framework.
  // The framework sets up both timer 2 and 
  // timer 1.  We use timer 2 to drive the
  // serial reading interrupt, so we can only
  // disable timer1 and its interrupt.

  // REALLY IMPORTANT NOTE - IF YOU WANT TO USE
  // ANALOGWRITE

  // Disabling timer 1 will switch off the timer
  // used for PWM, which underlies analogWrite.
  // If you want to use analogWrite, remove
  // the lines below.

  // DISABLE PWM COUNTER
  TCCR1B &= ~(1 << CS12);
  TCCR1B &= ~(1 << CS11);
  TCCR1B &= ~(1 << CS10);  
  // END DISABLE PWM COUNTER

  // Also, disable the interrupts on timer 0
  // REALLY IMPORTANT NOTE IF YOU WANT TO USE
  // DELAY remove this line
//   TIMSK0 &= ~(1 << TOIE0);
   
   /// XY init seq CLR - after INT INIT to ensure proper startup
    clearRows();
//    Serial.println("testing 1 2");
}


///////////////////////////////////////////////////////////////////////
// Global variables 

byte byte0, byte1, row = 0;                      // storage for incoming serial data

boolean WaitingCommand = true;          // true when we expect the next serial byte to be an address value, false otherwise
int command =  0;                       // garbage int to hold address of function

byte state = 0x00;                      // garbage byte to hold state value
byte x = 0x00;                          // garbage byte to hold x position
byte y = 0x00;                          // garbage byte to hold y position
byte z = 0x00;                          // garbage byte to iterate over buttons




// The following variables are used to store flags that indicate whether we have received a given message,
// the value of the data in that message.  e.g. IntensityChange = 0 == no intensity change message received,
// IntensityChange = 1 == an intensity change message has been received, and it's value will be in IntensityVal
// The code that eventually acts upon the incoming message will reset the 'Change' variable to 0 once the 
// message has been handled.            

int ShutdownModeChange = 0;            
boolean ShutdownModeVal = false;

int i = 0;                              // temporary variable for looping etc.
int id = 0;                             // temporary storage for incoming message ID
int curInt = 15;
int prevInt = 15;

unsigned long interval = 12;  // debounce time CAN BE ALTERED
unsigned long previousMillis=0; // millis() returns an unsigned long.

// These variables are used to handle the tilt messages, and store which ports are currently enabled,
// and which are not.
int tiltEnable = 0;
byte tiltNum;
byte tiltState;
const int xPin = A3;                  // x-axis of the accelerometer - CHANGE these as necessary
const int yPin = A2;                  // y-axis
const int zPin = A1;                  // z-axis (only on 3-axis models)

byte ADCnum;
byte ADCstate;
byte ADCEnableState[4];

///////////////////////////////////////////////
/// LED Functions
void setRows() {
      for (uint8_t y=0; y<8; y++) {
          for (uint8_t x=0; x<8; x++) {
                trellis.setLED(trellisXY.getTrellisId(x, y));
                trellis.writeDisplay();
     }
  }
}

void clearRows() {
      for (uint8_t x=0; x<8; x++) {
          for (uint8_t y=0; y<8; y++) {
              trellis.clrLED(trellisXY.getTrellisId(x, y));
              trellis.writeDisplay();
          }
      }
  }

////////////////////////////////////////////////
/// RACS - button states

    void checkButtons () {
      if ((unsigned long)(millis() - previousMillis) >= interval) {
        byte xVal;
        byte yVal;
                
        if (trellis.readSwitches()) {

          for (uint8_t i=0; i<NKEYS; i++) {

            if (trellis.justPressed(i)) {
               xVal = trellisXY.getTrellisX(i);
               yVal = trellisXY.getTrellisY(i);     
               Serial.write((0 << 4) | (1 & 15)); 
               Serial.write((yVal << 4) | (xVal & 15));              
            }
            if (trellis.justReleased(i)) {
               xVal = trellisXY.getTrellisX(i);
               yVal = trellisXY.getTrellisY(i);
               Serial.write((0 << 4) | (0 & 15)); 
               Serial.write((yVal << 4) | (xVal & 15));
       	   }
          }
        }
         previousMillis = millis();
      }
    }

///////////////////////////////////////////////
/// RACS JUST Check Serial now (pretty much straight out of the Arduinome)
/// This should eventually be re-implemented as an interrupt, it seems to lag a little

void checkSerial() {
    
    do {
      if (Serial.available()) {
        if (WaitingCommand) {          // command is the first byte of the two byte command
          byte0 = Serial.read();
          command = byte0 >> 4;
          WaitingCommand = false;
        }

           if(Serial.available()) { // command outside range so must be an error
              WaitingCommand = true; // the next byte is the first byte of the next command
              byte1 = Serial.read(); // read the second byte of this command
 
              switch(command) {
 
                case 2: // LED command
                state = byte0 & 0x0f; // is the command for on or off
                x = ((byte1 >> 4) & 0x07); // mask so we don't go over the 8 by 8 grid
                y = ((byte1 & 15) & 0x07);
 
                  if (state == 0) {
                      trellis.clrLED(trellisXY.getTrellisId(y, x)); //offLED(x, y);
                 }
                   else {
                   trellis.setLED(trellisXY.getTrellisId(y, x)); //onLED(x, y);
                   }
                  break;
 
                case 3: // led intensity command
                  curInt = (byte1 & 0x0f);
                break;
                 
                case 4: // led test command
                  if( (byte1 & 1) == 0) {
                    clearRows();
                   }
                  else {
                    setRows();
                   }
                break;
                
                case 5: // enable tilt command
                  state = byte1 & 0x0F;
                  ADCEnableState[(byte1 >> 4)&0x03] = state;
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
                    trellis.setLED(trellisXY.getTrellisId(y, x)); //onLED(x, y);
                   }
                  else {
                       trellis.clrLED(trellisXY.getTrellisId(y, x)); // offLED(x, y);
                  }
                  x++;
                }
                break;
 
                case 8: // colomn command
                  x = byte0 & 0x07; // mask this value so we don't write to an invalid address.
                  y = 0;
                  for(byte i = 0x1; i != 0x0; i <<= 1 ) {
                    if( (i & byte1) != 0) {
                      trellis.setLED(trellisXY.getTrellisId(y, x)); //onLED(x, y);
                    }
                    else {
                      trellis.clrLED(trellisXY.getTrellisId(y, x)); //offLED(x, y);
                    }
                  y++;
                }
                break;
                
               } // end switch(command)
              } // end of else in if(WaitingCommand)
             } // end of if(Serial.available();
            } // end of do
           while (Serial.available() > 16); // keep on going if we have a lot of commands stacked up
          }

/// ADC commands
void sendADC(int port, int value) {
  Serial.write((1 << 4) | ((port << 2) & 0x0C) | ((value >> 8) & 0x03));
  Serial.write(value & 0xFF);
}

int current[4]; 
int previous[4]; 
int tolerance = 7;

void checkADCs() {

  for (int adcCounter = 0; adcCounter < 4; adcCounter++)
  {

    if (ADCEnableState[adcCounter] != 0)
    {
      current[adcCounter] = analogRead(adcCounter);

      if (abs(previous[adcCounter]-current[adcCounter]) > tolerance)
      {
        previous[adcCounter] = current[adcCounter];
        sendADC(adcCounter,current[adcCounter]);
      }
    }
  }
}

///// /TILT - not yet
//void sendTilt(int x, int y, int z) {
//      Serial.write((8 << 4) | ((1 << 2) & 0x0C) | ((x >> 8) & 0x03));
//      Serial.write(x & 0xFF);
//      Serial.write(y & 0xFF);
//      Serial.write(z & 0xFF); 
////        byte xyz = (
////        Serial.write(nxyz, sizeof(nxyz))
//}
//  
//int cur[3];  // val[3]
//int prev[3]; 
//int tol = 7;
//int aX;
//int aY;
//int aZ;
//
//void checkTilt() {
//    if (tiltEnable != 0) {
//      aX = analogRead(xPin);
//      aY = analogRead(yPin);
//      aZ = analogRead(zPin);
//      sendTilt(aX,aY,aZ);
//      }
//}


/// RACS - Send it 
void loop() {
  checkSerial();
  checkButtons();
  trellis.writeDisplay();
//  checkTilt();
  checkADCs();
  
  /// Intensities - sets ALL LEDs only until the Trellis lib is edited
  if (curInt != prevInt) {
    trellis.setBrightness(curInt); 
    curInt = prevInt;
  }
 }
 
//eof
