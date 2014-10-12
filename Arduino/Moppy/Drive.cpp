
#include "Drive.h"

Drive::Drive(): 
stepPin(-1), dirPin(-1), maxPos(0){
}

Drive::Drive(unsigned int stepPin, unsigned int dirPin, unsigned int maxPos) : 
stepPin(stepPin), dirPin(dirPin), maxPos(maxPos) {
  switching = false;
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  reset();
  setPos(40);
}

void Drive::play() {
  if (period > 0){
    switched = false;
    tick++;
    if (tick >= period){
      togglePin();
      tick = 0;
    }
  } 
  else if(switching) {
    if (dir == LOW) {
      if (switched == false && pos > maxPos/2) {
        switched = true;
        digitalWrite(dirPin, HIGH);
        dir = HIGH;
      }
    } 
    else if (switched == false && pos < maxPos/2) {
      switched = true;
      digitalWrite(dirPin, LOW);
      dir = LOW;
    }
  } 
}

void Drive::togglePin() {
  //Switch directions if end has been reached
  if (pos >= maxPos) {
    dir = HIGH;
    digitalWrite(dirPin, HIGH);
  } 
  else if (pos <= 0) {
    dir = LOW;
    digitalWrite(dirPin, LOW);
  }

  //Update currentPosition
  if (dir == HIGH){
    pos--;
  } 
  else {
    pos++;
  }

  //Pulse the control pin
  
  digitalWrite(stepPin, state);
  state = ~state;
}

void Drive::togglePinBAF() {
  //Switch directions if end has been reached
  if (state == LOW) {
    dir = ~dir;
    digitalWrite(dirPin, dir);
  }

  //Update currentPosition
  if (dir == HIGH){
    pos--;
  } 
  else {
    pos++;
  }

  //Pulse the control pin
  
  digitalWrite(stepPin, state);
  state = ~state;
}

void Drive::setPos(int newPos) {
  if (newPos > pos) {
    digitalWrite(dirPin, FORWARD);
    dir = FORWARD;
  } else {
    digitalWrite(dirPin, REVERSE);
    dir = REVERSE;
  }
  int diff = abs(newPos - pos);
  for (byte s=0; s < diff; s+=2) {
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin, LOW);
    delay(5);
  }
  pos = newPos;
}

void Drive::reset() {
  digitalWrite(dirPin, HIGH); // Go in reverse
  for (byte s=0; s < maxPos; s+=2) { //Half max because we're stepping directly (no toggle)
    digitalWrite(stepPin, HIGH);
    digitalWrite(stepPin, LOW);
    delay(5);
  }
  digitalWrite(dirPin, LOW);
  period = 0;
  pos = 0;
  tick = 0;
  state = LOW;
  dir = LOW;
  switched = false;
}

boolean Drive::isAvailable() {
  return period == 0;
}

