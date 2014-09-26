#include <TimerOne.h>
#include <LiquidCrystal.h>

boolean firstRun = true; // Used for one-run-only stuffs;

#define RESOLUTION 40 //Microsecond resolution for notes

// drive goes forward with dir pin low
#define FORWARD LOW
// drive goes backward with dir pin high
#define REVERSE HIGH


/*NOTE: Many of the arrays below contain unused indexes.  This is 
 to prevent the Arduino from having to convert a pin input to an alternate
 array index and save as many cycles as possible.  In other words information 
 for pin 2 will be stored in index 2, and information for pin 4 will be 
 stored in index 4.*/


/*An array of maximum track positions for each step-control pin.  Even pins
 are used for control, so only even numbers need a value here.  3.5" Floppies have
 80 tracks, 5.25" have 50.  These should be doubled, because each tick is now
 half a position (use 158 and 98).
 */
const byte MAX_POSITION[] = {
  0,0,158,0,158,0,158,0,158,158,158,0,158,0,158,0,158,0};

//Array to track the current position of each floppy head.  (Only even indexes (i.e. 2,4,6...) are used)
byte currentPosition[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

/*Array to keep track of state of each pin.  Even indexes track the control-pins for toggle purposes.  Odd indexes
 track direction-pins.  LOW = forward, HIGH=reverse
 */
int currentState[] = {
  0,0,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW,LOW
};

//Current period assigned to each pin.  0 = off.  Each period is of the length specified by the RESOLUTION
//variable above.  i.e. A period of 10 is (RESOLUTION x 10) microseconds long.
unsigned int currentPeriod[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

//Current tick
unsigned int currentTick[] = {
  0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 
};

boolean switched[] = {
  true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true,true};

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 7, 6, 5, 4);

class Drive {
  unsigned int period, pos, tick, state, dir;
  const unsigned int stepPin, dirPin, maxPos;
  boolean switched;
public:
  Drive(unsigned int stepPin, unsigned int dirPin, unsigned int maxPos = 158) : 
  stepPin(stepPin), dirPin(dirPin), maxPos(maxPos) {
    reset();
  }

  void play() {
    if (period > 0){
      switched = false;
      tick++;
      if (tick >= period){
        togglePin();
        tick = 0;
      }
    } 
    else {
      if (dir == FORWARD) {
        if (switched == false && pos > maxPos/2) {
          switched = true;
          digitalWrite(dirPin, REVERSE);
          dir = REVERSE;
        }
      } 
      else if (switched == false && pos < maxPos/2) {
        switched = true;
        digitalWrite(dirPin, FORWARD);
        dir = FORWARD;
      }
    } 
  }

  void togglePin() {
    //Switch directions if end has been reached
    if (pos >= maxPos) {
      dir = REVERSE;
      digitalWrite(dirPin, REVERSE);
    } 
    else if (pos <= 0) {
      dir = FORWARD;
      digitalWrite(dirPin, FORWARD);
    }

    //Update currentPosition
    if (dir == REVERSE){
      pos--;
    } 
    else {
      pos++;
    }

    //Pulse the control pin
    state = ~state;
    digitalWrite(stepPin, state);
  }

  void reset() {
    digitalWrite(dirPin, REVERSE); // Go in reverse
    for (byte s=0; s < maxPos; s+=2) { //Half max because we're stepping directly (no toggle)
      digitalWrite(stepPin, HIGH);
      digitalWrite(stepPin, LOW);
      delay(5);
    }
    digitalWrite(dirPin, FORWARD);
    period = 0;
    pos = 0;
    tick = 0;
    state = LOW;
    dir = FORWARD;
    switched = false;
  }
};


//Setup pins (Even-odd pairs for step control and direction
void setup(){
  lcd.begin(16, 2);
  pinMode(2, OUTPUT); // Step control 4
  pinMode(3, OUTPUT); // Direction 4
  pinMode(8, OUTPUT); // Step control 4
  pinMode(9, OUTPUT); // Direction 4
  pinMode(10, OUTPUT); // Direction 4
  pinMode(13, OUTPUT); // Direction 4

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

}


//Resets all the pins
void resetAll(){


}







