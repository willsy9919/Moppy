#include <TimerOne.h>
#include <LiquidCrystal.h>
#include "Drive.h"

#define RESOLUTION 40 //Microsecond resolution for notes

#define TOTAL_DRIVES 8

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

Drive drives[TOTAL_DRIVES];

//Setup pins (Even-odd pairs for step control and direction
void setup(){
  lcd.begin(16, 2);

  drives [0] = Drive(52, 53);
  drives [1] = Drive(50, 51);
  drives [2] = Drive(48, 49);
  drives [3] = Drive(46, 47);
  
  drives [4] = Drive(24, 25);
  drives [5] = Drive(26, 27);
  drives [6] = Drive(28, 29);
  drives [7] = Drive(22, 23);

  Timer1.initialize(RESOLUTION); // Set up a timer at the defined resolution
  Timer1.attachInterrupt(tick); // Attach the tick function

  Serial.begin(9600);
  delay(2000);
}


void loop(){
  
  //Only read if we have 
  if (Serial.available() > 2){
    //Watch for special 100-message to reset the drives
    if (Serial.peek() == 100) {
      resetAll();
      //Flush any remaining messages.
      while(Serial.available() > 0){
        Serial.read();
      }
    } 
    else{
      int channel = Serial.read();
      unsigned int period = (Serial.read() << 8) | Serial.read();
      drives[channel/2-1].period = period;
    }
  }
}


/*
Called by the timer inturrupt at the specified resolution.
 */
void tick()
{
  for (int i = 0; i < TOTAL_DRIVES; i++) {
    drives[i].play(); 
  }
}


//Resets all the pins
void resetAll(){
  for (int i = 0; i < TOTAL_DRIVES; i++) {
    drives[i].reset(); 
  }
}








