#include "arduino.h"

#ifndef Drive_h
#define Drive_h

// drive goes forward with dir pin low
#define FORWARD LOW
// drive goes backward with dir pin high
#define REVERSE HIGH

class Drive {
  unsigned int pos, tick, state, dir;
  unsigned int stepPin, dirPin, maxPos;
  boolean switched, switching;
public:
  unsigned int period;
  
  // default constructor required for creating a array of drives. It should never be used for an actual drive
  Drive();
  Drive(unsigned int stepPin, unsigned int dirPin, unsigned int maxPos = 158);

  void play();

  void togglePin();
  void togglePinBAF();

  void setPos(int newPos);

  void reset();
  
  boolean isAvailable();
};

#endif

