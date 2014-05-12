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
Uses Trellis XY Library and monomecereal.maxpat, but it will also work 
with monome's serialOSC if the USB chip is flashed. A 3D printed enclosure 
was adapted from Adafruit's implementation. All files are on the git.

This sketch was largely based on the Arduinome clone originally by Owen 
Vallis & Jordan Hochenbaum: http://flipmu.com/work/arduinome/

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
    
    
    unsigned long interval = 12;  // the time we need to wait CAN BE ALTERED (used in debounce and LED tests) 
    unsigned long previousMillis=0; // millis() returns an unsigned long.
        

    /// SETUP    
void setup() {
      Serial.begin(115200);
      Serial.println("racs arduinome");
     
      // INT pin requires a pullup
      pinMode(INTPIN, INPUT);
      digitalWrite(INTPIN, HIGH);
     
      // order is important here!! Start with tile address you want to
      // use as the first one, etc.
      trellis.begin(0x73, 0x72, 0x70, 0x71);
    
  /// XY init seq SET
   setRows();
    
  /// INTERRUPT INIT
  // Set up 8-bit counter 2, output compare switched off,
  // normal waveform generation (whatever that might mean)
  TCCR2A = 0;
  // set counter to be clocked at 16Mhz/8 = 2Mhz
  TCCR2B = 1<<CS21;

  // set the interrupt mask so that we get an interrupt
  // on timer 2 overflow, i.e. after 256 clock cycles.
  // The timer 2 interrupt routine will execute every
  // 128 uS.
  TIMSK2 = 1<<TOIE2;

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
  // note: using delay WILL SLOW DOWN THE CODE
//   TIMSK0 &= ~(1 << TOIE0);

   /// XY init seq CLR - after INT INIT to ensure proper startup
    clearRows();
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
boolean ShutdownModeVal= false;

int i = 0;                              // temporary variable for looping etc.
int id = 0;                             // temporary storage for incoming message ID

// These variables are used to handle the ADC messages, and store which ports are currently enabled,
// and which are not.
byte ADCnum;
byte ADCstate;
byte ADCEnableState[4];


////////////////////////////////////////////////
/// RACS - button states

    void checkButtons () {      /// RACS - WORKING
      if ((unsigned long)(millis() - previousMillis) >= interval) {
                
        if (trellis.readSwitches()) {

          for (uint8_t i=0; i<numKeys; i++) {

            if (trellis.justPressed(i)) {
               TrellisToXY(i, TileOffsets, &xVal, &yVal);      
               Serial.write((0 << 4) | (1 & 15));      // OG           Serial.write((0 << 4) | (b & 15));
               Serial.write((yVal << 4) | (xVal & 15));      // OG          Serial.write((id << 4) | (i & 15));              
            }
            if (trellis.justReleased(i)) {
               TrellisToXY(i, TileOffsets, &xVal, &yVal);
               Serial.write((0 << 4) | (0 & 15)); 
               Serial.write((yVal << 4) | (xVal & 15));
       	   }
          }
        }
         previousMillis = millis();
      }
    }
                          


///////////////////////////////////////////////
/// LED Functions

void clearRows() {
      for (uint8_t xVal=0; xVal<8; xVal++) {
        yVal=0;
          while (yVal<8) {
          if ((unsigned long)(millis() - previousMillis) >= interval) {
          int LED = XYToTrellis(numKeys, TileOffsets, xVal, yVal);
          trellis.clrLED(LED);
          trellis.writeDisplay();
          yVal++;
          previousMillis = millis();
          }
          }
      }
  }
 
void setRows() {
      for (uint8_t yVal=0; yVal<8; yVal++) {
          xVal=0;
          while (xVal<8) {
            if ((unsigned long)(millis() - previousMillis) >= interval) {
          int LED = XYToTrellis(numKeys, TileOffsets, xVal, yVal);
          trellis.setLED(LED);
          trellis.writeDisplay();
          xVal++;
          previousMillis = millis();
        }
     }
  }
}



///////////////////////////////////////////////
/// RACS JUST Check Serial now (pretty much straight out of the Arduinome)
/// This should eventually be re-implemented as an interrupt, it seems to lag a little

ISR(TIMER2_OVF_vect) {
  // first up: enable interrupts to keep the serial
  // class responsive
  sei(); 
    
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
                      trellis.clrLED(XYToTrellis(numKeys, TileOffsets, y, x)); //offLED(x, y);
                 }
                   else {
                   trellis.setLED(XYToTrellis(numKeys, TileOffsets, y, x)); //onLED(x, y);
                   }
                  break;
 
                case 3: // led intensity command
                break;
                 
                case 4: // led test command
                  if( (byte1 & 1) == 0) {
                    clearRows();
                 }
                  else {
                    setRows();
                 }
                break;
                
                case 5: // enable ADC command - but we don't do this
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
                    trellis.setLED(XYToTrellis(numKeys, TileOffsets, y, x)); //onLED(x, y);
                   }
                  else {
                       trellis.clrLED(XYToTrellis(numKeys, TileOffsets, y, x)); // offLED(x, y);
                  }
                  x++;
                }
                break;
 
                case 8: // colomn command
                  x = byte0 & 0x07; // mask this value so we don't write to an invalid address.
                  y = 0;
                  for(byte i = 0x1; i != 0x0; i <<= 1 ) {
                    if( (i & byte1) != 0) {
                      trellis.setLED(XYToTrellis(numKeys, TileOffsets, y, x)); //onLED(x, y);
                    }
                    else {
                      trellis.clrLED(XYToTrellis(numKeys, TileOffsets, y, x)); //offLED(x, y);
                    }
                  y++;
                }
                break;
               } // end switch(command)
              } // end of else in if(WaitingCommand)
             } // end of if(Serial.available();
            } // end of do
           while (Serial.available() > 8); // keep on going if we have a lot of commands stacked up
          }


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


/// RACS - Send it 
void loop() {
  checkButtons();
  trellis.writeDisplay();
  checkADCs();
  
 }

// eof
