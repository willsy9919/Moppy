
#include <TimerOne.h>
boolean firstRun = true; // Used for one-run-only stuffs;
//First pin being used for floppies, and the last pin. Used for looping over all pins.
const byte FIRST_PIN = 22;
const byte PIN_MAX = 53;
#define RESOLUTION 40 //Microsecond resolution for notes
/*NOTE: Many of the arrays below contain unused indexes. This is
 to prevent the Arduino from having to convert a pin input to an alternate
 array index and save as many cycles as possible. In other words information
 for pin 2 will be stored in index 2, and information for pin 4 will be
 stored in index 4.*/
/*An array of maximum track positions for each step-control pin. Even pins
 are used for control, so only even numbers need a value here. 3.5" Floppies have
 80 tracks, 5.25" have 50. These should be doubled, because each tick is now
 half a position (use 158 and 98).
 */

byte MAX_POSITION[] = {
  0,0,158,0,158,0,158,0,158,0,158,0,158,0,158,0,158,0};
//Array to track the current position of each floppy head. (Only even indexes (i.e. 2,4,6...) are used)
byte currentPosition[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
/*Array to keep track of state of each pin. Even indexes track the control-pins for toggle purposes. Odd indexes
 track direction-pins. LOW = forward, HIGH=reverse
 */
int currentState[] = {
  0,0,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW
};
//Current period assigned to each pin. 0 = off. Each period is of the length specified by the RESOLUTION
//variable above. i.e. A period of 10 is (RESOLUTION x 10) microseconds long.
unsigned int currentPeriod[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};
//Current tick
unsigned int currentTick[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

//Setup pins (Even-odd pairs for step control and direction
void setup(){
  pinMode(22, OUTPUT); // Step control 1
  pinMode(23, OUTPUT); // Direction 1
  pinMode(24, OUTPUT); // Step control 2
  pinMode(25, OUTPUT); // Direction 2
  pinMode(26, OUTPUT); // Step control 3
  pinMode(27, OUTPUT); // Direction 3
  pinMode(28, OUTPUT); // Step control 4
  pinMode(29, OUTPUT); // Direction 4
  pinMode(53, OUTPUT); // Step control 5
  pinMode(52, OUTPUT); // Direction 5
  pinMode(51, OUTPUT); // Step control 6
  pinMode(50, OUTPUT); // Direction 6
  pinMode(49, OUTPUT); // Step control 7
  pinMode(48, OUTPUT); // Direction 7
  pinMode(47, OUTPUT); // Step control 8
  pinMode(46, OUTPUT); // Direction 8

  Timer1.initialize(RESOLUTION); // Set up a timer at the defined resolution
  Timer1.attachInterrupt(tick); // Attach the tick function
  Serial.begin(9600);
}
void loop(){
  //The first loop, reset all the drives, and wait 2 seconds...
  if (firstRun)
  {
    firstRun = false;
    resetAll();
    delay(2000);
  }
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
      currentPeriod[Serial.read()] = (Serial.read() << 8) | Serial.read();
    }
  }
}
/*
Called by the timer inturrupt at the specified resolution.
 */
void tick()
{
  /*
If there is a period set for control pin 2, count the number of
   ticks that pass, and toggle the pin if the current period is reached.
   */
  if (currentPeriod[2]>0){
    currentTick[2]++;
    if (currentTick[2] >= currentPeriod[2]){
      togglePin(22,23,2);
      currentTick[2]=0;
    }
  }
  if (currentPeriod[4]>0){
    currentTick[4]++;
    if (currentTick[4] >= currentPeriod[4]){
      togglePin(24,25,4);
      currentTick[4]=0;
    }
  }
  if (currentPeriod[6]>0){
    currentTick[6]++;
    if (currentTick[6] >= currentPeriod[6]){
      togglePin(26,27,6);
      currentTick[6]=0;
    }
  }
  if (currentPeriod[8]>0){
    currentTick[8]++;
    if (currentTick[8] >= currentPeriod[8]){
      togglePin(28,29,8);
      currentTick[8]=0;
    }
  }
  if (currentPeriod[10]>0){
    currentTick[10]++;
    if (currentTick[10] >= currentPeriod[10]){
      togglePin(52,53,10);
      currentTick[10]=0;
    }
  }
  if (currentPeriod[12]>0){
    currentTick[12]++;
    if (currentTick[12] >= currentPeriod[12]){
      togglePin(50,51,12);
      currentTick[12]=0;
    }
  }
  if (currentPeriod[14]>0){
    currentTick[14]++;
    if (currentTick[14] >= currentPeriod[14]){
      togglePin(48,49,14);
      currentTick[14]=0;
    }
  }
  if (currentPeriod[16]>0){
    currentTick[16]++;
    if (currentTick[16] >= currentPeriod[16]){
      togglePin(46,47,16);
      currentTick[16]=0;
    }
  }
}
void togglePin(byte pin, byte direction_pin, int i) {
  //Switch directions if end has been reached
  if (currentPosition[i] >= MAX_POSITION[i]) {
    currentState[i+1] = HIGH;
    digitalWrite(direction_pin,HIGH);
  }
  else if (currentPosition[i] <= 0) {
    currentState[i+1] = LOW;
    digitalWrite(direction_pin,LOW);
  }
  //Update currentPosition
  if (currentState[i+1] == HIGH){
    currentPosition[i]--;
  }
  else {
    currentPosition[i]++;
  }
  //Pulse the control pin
  digitalWrite(pin,currentState[i]);
  currentState[i] = ~currentState[i];
}

void togglePinBaF(byte pin, byte direction_pin, int i) {
  //Pulse the control pin
  digitalWrite(pin,HIGH);
  digitalWrite(pin,LOW);

  currentState[i+1] = ~currentState[i+1];
  digitalWrite(direction_pin, currentState[i+1]);
}
//
//// UTILITY FUNCTIONS
//
//Not used now, but good for debugging...
void blinkLED(){
  digitalWrite(13, HIGH); // set the LED on
  delay(250); // wait for a second
  digitalWrite(13, LOW);
}
//For a given controller pin, runs the read-head all the way back to 0
void reset(byte pin, int i)
{
  digitalWrite(pin+1,HIGH); // Go in reverse
  for (byte s=0;s<MAX_POSITION[i];s+=2){ //Half max because we're stepping directly (no toggle)
    digitalWrite(pin,HIGH);
    digitalWrite(pin,LOW);
    delay(5);
  }
  currentPosition[i] = 0; // We're reset.
  digitalWrite(pin+1,LOW);
  currentPosition[i+1] = 0; // Ready to go forward.
}
//Resets all the pins
void resetAll(){

  reset(22,2);
  reset(24,4);
  reset(26,6);
  reset(28,8);
  reset(52,10);
  reset(50,12);
  reset(48,14);
  reset(46,16);

}






