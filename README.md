trellis-monome
==============

A monome clone using the Adafruit Trellis keypads

This project was largely based on the Arduinome by Owen Vallis & 
Jordan Hochenbaum: http://flipmu.com/work/arduinome/

Many thanks to Brian Crabtree and Kelli Cain as well as the rest of the monome community for creating supporting open source hardware/software: http://monome.org/


Robin Stubbs - robin.stubbs@uleth.ca

Click "Download ZIP" to get all files ------------------->

***
This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.
***

NOTE: For now you will need Cycling '74 's Max (http://cycling74.com/downloads/), but only the runtime version (free!) in order to run the monomecereal.maxpat

INSTRUCTIONS:

1. Set up Trellis keypads according to adafruit: https://learn.adafruit.com/adafruit-trellis-diy-open-source-led-keypad?view=all

2. Place the Trellis_XY library by George Kuetemeyer in your libraries folder. Test your Trellis keypads using the test sketches - ensure that all LEDs are lighting up in order. If they are jumping from one keypad to another, change the order of addresses under trellis.begin()

3. Upload the latest trellis-monome firmware to your arduino. Start monomecereal.maxpat and choose your serial port (it will be the same as the one you used in the Arduino IDE). Click 'serial on' and 'osc on'

4. Open trellis-monome test.maxpatch and press your keys. Watch the buttons light up! If the right buttons are not lighting check the cable orientation in monomecereal.maxpat

5. You are done! download apps for the monome at http://monome.org/docs/app 

OPTIONAL: use the .stl files in the 3D folder and 3D print yourself an enclosure! 