#include <JC_Button.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

int state = 0;
//Globals for ColorSensor
// library notes: Pick analog outputs, for the UNO these 3, 5, 6.
// use ~560  ohm resistor between Red & Blue, ~1K for green (its brighter)
// 10 A4-A7

#define redpin 3
#define greenpin 5
#define bluepin 6
#define onBoardLed 7
#define commonAnode true

int redLed = 10;
int greenLed = 12;
int blueLed = 11;

// From library: our RGB -> eye-recognized gamma color
byte gammatable[256];
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X);

int relayPinRed = 2;
int relayPinGreen = 8;
int relayPinBlue = 9;
int relayPinFinal = 4;

//Globals Simon Says button library setup
Button redBtn(A1, 200);
Button greenBtn(A2, 200);
Button blueBtn(A3, 200);
Button resetBtn(A0, 200);

//calibration values
int redThreshold = 200;
int greenThreshold = 150;
int blueThreshold = 100;

//employee reset variable
int timeReset = 0;

//Game vars
const int sizeOfSeq = 9;
int correctOrder[sizeOfSeq + 1] = {1, 2, 2, 3, 1, 3, 3, 2, 1};
int guessed[sizeOfSeq + 1] = {0,0,0}; // red = 1 green = 2 blue = 3 zero is null
//game logic vars
int isRedTriggered = 0;
int isGreenTriggered = 0;
int isBlueTriggered = 0;

int counter = 0;
int delayTime = 250;
int started = 1;

//Helper Funcs
void emergencyResetCheck(){
  if(!resetBtn.isPressed()){
        Serial.println("EMERGENCY RESET");
        timeReset++;
       } else {
        timeReset = 0;
       }


   if(timeReset > 100){
    Serial.println("Pressed long enough");
    delay(1000);
    timeReset = 0;
    state = 0;
  }
}

void onSuccess(){
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, HIGH);
    digitalWrite(blueLed, HIGH);
    delay(200);
    
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    delay(200);

    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, HIGH);
    digitalWrite(blueLed, HIGH);
    delay(200);
    
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    delay(200);
    
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, HIGH);
    digitalWrite(blueLed, HIGH);
    delay(200);

    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    delay(200);
    
    digitalWrite(redLed, HIGH);
    digitalWrite(greenLed, HIGH);
    digitalWrite(blueLed, HIGH);
    delay(200);

    //Open 12v lock
    digitalWrite(relayPinFinal, HIGH);
    delay(60);
    digitalWrite(relayPinFinal, LOW);
}

int checkGuess(){
  for(int i = 0; i < sizeOfSeq; i++){
    if(guessed[i] != correctOrder[i]){
      return 0;
    }
  }
  return 1;
}

void popLockForColor(int color){
  // red is 1, green is 2, blue is 3
  switch(color){
    case 1:
    //pop red lock and turn red LED on.
    digitalWrite(redLed, HIGH);
    digitalWrite(relayPinRed, HIGH);
    delay(30);
    digitalWrite(relayPinRed, LOW);
    break;
    case 2:
    digitalWrite(greenLed, HIGH);
    digitalWrite(relayPinGreen, HIGH);
    delay(30);
    digitalWrite(relayPinGreen, LOW);
    break;
    case 3:
    digitalWrite(blueLed, HIGH);
    digitalWrite(relayPinBlue, HIGH);
    delay(30);
    digitalWrite(relayPinBlue, LOW);
    break;
    default:
    break;
  }
}

void setup() {
  Serial.begin(9600);
  Serial.println("Started");
  
 //
   //Color sensor Setup
 //
  if (tcs.begin()) {
    Serial.println("Found sensor");
  } else {
    Serial.println("TCS34725 not found ... check your connections");
    while (1); // halt!
  }

  resetBtn.begin();
  redBtn.begin();
  greenBtn.begin();
  blueBtn.begin();

  pinMode(redpin, OUTPUT);
  pinMode(greenpin, OUTPUT);
  pinMode(bluepin, OUTPUT);

  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
  pinMode(onBoardLed, OUTPUT);
  
  pinMode(relayPinRed, OUTPUT);
  pinMode(relayPinGreen, OUTPUT);
  pinMode(relayPinBlue, OUTPUT);
  pinMode(relayPinFinal, OUTPUT);
  
  digitalWrite(redpin, HIGH);
  digitalWrite(greenpin, HIGH);
  digitalWrite(bluepin, HIGH);
  delay(1000);
  digitalWrite(redpin, LOW);
  digitalWrite(greenpin, LOW);
  digitalWrite(bluepin, LOW);

  // Gamma calc
    for (int i=0; i<256; i++) {
      float x = i;
      x /= 255;
      x = pow(x, 2.5);
      x *= 255;
  
      if (commonAnode) {
        gammatable[i] = 255 - x;
      } else {
        gammatable[i] = x;
      }
    }
}
void loop() {
  //State Machine
  SM();
}

void SM(){
  switch(state){
    case 0:
    //reset some stuff 
    Serial.println("YOU RESET THE GAME");
    counter = 0;
    isRedTriggered = 0;
    isGreenTriggered = 0;
    isBlueTriggered = 0;
    timeReset = 0;
    
    digitalWrite(onBoardLed, LOW);
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    delay(1000);
    
    started = 1;
    state = 1;
    break;
    
    case 1:
    resetBtn.read();
    
    if(resetBtn.isPressed() && started){
      Serial.println("Game started over and the reset must be pressed to start a new game.");
    } else {

      emergencyResetCheck();
       
      started = 0;
       Serial.println("In state 1");
      float red, green, blue;
      digitalWrite(onBoardLed, HIGH);
    
      delay(60);  // takes 50ms to read
    
      tcs.getRGB(&red, &green, &blue);

    if((int(red) > 110 && int(green) < 70 && int(blue) < 70) && !isRedTriggered){
      Serial.println("This is RED");
      //Turn redLED on
      isRedTriggered = 1;
      counter++;
      //Open red Door
      popLockForColor(1);
    } else if(int(red) < 70 && int(green) > 101 && int(blue) < 100 && !isGreenTriggered){
      Serial.println("This is GREEN");
      //Turn greenLED on
      isGreenTriggered = 1;
      counter++;
      popLockForColor(2);
    } else if (int(red) < 70 && int(green) < 87 && int(blue) > 90 && !isBlueTriggered){
      Serial.println("This is BLUE");
      //Turn BlueLED on
      isBlueTriggered = 1;
      counter++;
      popLockForColor(3);
    }

    if(isRedTriggered && isGreenTriggered && isBlueTriggered & counter == 3){
      state = 2;
      break;
    }

      analogWrite(redpin, gammatable[(int)red]);
      analogWrite(greenpin, gammatable[(int)green]);
      analogWrite(bluepin, gammatable[(int)blue]);

    }
   

    break;
    
    case 2:
    Serial.println("In state 2");
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);

    delay(500);
    
    digitalWrite(onBoardLed, LOW);

    //Simon Says Game
    Serial.println("Simon Says Game Part now playing sequence");
    
    for(int i = 0; i < sizeOfSeq; i++){
      delay(delayTime);
      switch(correctOrder[i]){
        case 1:
          digitalWrite(redLed, HIGH);
          delay(delayTime);
          digitalWrite(redLed, LOW);
        break;
        
        case 2:
          digitalWrite(greenLed, HIGH);
          delay(delayTime);
          digitalWrite(greenLed, LOW);
        break;

        case 3:
          digitalWrite(blueLed, HIGH);
          delay(delayTime);
          digitalWrite(blueLed, LOW);
        break;
        
        default:
        break;
      }
      counter = 0;
      state = 3;
    }
    break;

    case 3:
    //button time
    Serial.println("In state 3");

    emergencyResetCheck();

    int resetBtnPressed = !resetBtn.read();
    int redBtnPressed = redBtn.read();
    int greenBtnPressed = greenBtn.read();
    int blueBtnPressed = blueBtn.read();
    
    digitalWrite(redLed, LOW);
    digitalWrite(greenLed, LOW);
    digitalWrite(blueLed, LOW);
    
    if(resetBtn.wasPressed()){
      state = 2;
      Serial.println("YOU PRESSED RESET");
      }
    
    if(redBtn.wasPressed()){
      Serial.println("YOU PRESSED RED");
      digitalWrite(redLed, HIGH);
      delay(100);
      digitalWrite(redLed, LOW);
      guessed[counter] = 1;
      counter++;
    }

    if(greenBtn.wasPressed()){
      Serial.println("YOU PRESSED GREEN");
      digitalWrite(greenLed, HIGH);
      delay(100);
      digitalWrite(greenLed, LOW);
      guessed[counter] = 2;
      counter++;
    }

    if(blueBtn.wasPressed()){
      Serial.println("YOU PRESSED BLUE");
      digitalWrite(blueLed, HIGH);
      delay(100);
      digitalWrite(blueLed, LOW);
      guessed[counter] = 3;
      counter++;
    }

    if(counter == sizeOfSeq){

      //check that answer
      int checked = checkGuess();
      if(checked){
        Serial.println("You Won going to state 4");
       // pop Lock and doreset
       started = 1;
       delay(1000);
       onSuccess();
       state = 0;
      } else {
        Serial.println(counter);
        Serial.println("You lost going to state 2");
        //lost
        //wrongLights and buzzer that sounds horrible
        counter = 0;
        state = 2;
        Serial.println(counter);
      }
    }
    break;
    default:
    break;
  }
}
