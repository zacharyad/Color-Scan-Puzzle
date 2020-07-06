# COLOR SCANNING to SIMON-SAYS PUZZLE

### This is a puzzle that uses hardware and software to create an engaging puzzle thaty acts as one puzzle inside a larger escape room.

## The User experience:

-Users unlocks a box that has an exposed color sensing chip (covered by plexi-glass) thats says "color Scan" as well as three LEDs labeled "R", "G", and "B" respectively.
-When any object that meets the requirements of red, green, or blue pigmintation the corresponding LED will light up and a lock inside the room will open a door revealing that same color of button.
-When a red, green, and blue object is scanned, and all buttons are exposed/unlocked, the box previously used to scan colors will begin showing a sequence of blinks from the RGB LEDS. This sequence will then be mimic-ed with button presses to input the correct "code" that was previously "blinked out" from the LEDs.
-If done correctly a new area will be unlocked, concluding this puzzle.
-Reseting the puzzle is done using a reset button that customers can use to start simon says sequence over again. Hard-reset can be used by employees to get puzzle back to state 0, using same button but with a long hold of 15 seconds. This employee reset is needed for possible isses with locks being closed, not allowing for completion of puzzle in simon says mode leading to a "jammed" state.

## The Hardware:

-Color sensing module (TCS34725).
-Arduino Nano.
-5v Relays.
-12v Locks.
-LEDs (Red, Green, Blue).
-Momentary push buttons.

## The Software:

-Code written by myself using arduino IDE, C/C++.
-Libraries:
-JC_Button.h: creates a clean (cleaner code) interaction with physical buttons, debounce functionality was biggest reason for its use.
-Wire.h and Adafruit_TCS34725.h: both used to communicate with color sensing module easier

<img src="./colorSensingHousing.jpg" alt="Hardware in mid-creation, before install" width="250" height="250">

<img src="./colorSensingHousingInside.jpg" alt="Hardware in mid-creation, inside components, before install" width="250" height="250">
