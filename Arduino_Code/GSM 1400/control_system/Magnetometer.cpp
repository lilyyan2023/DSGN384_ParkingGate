/*
  Morse.cpp - Library for flashing Morse code.
  Created by David A. Mellis, November 2, 2007.
  Released into the public domain.



#include "Arduino.h"
#include "Magnetometer.h"

QMC5883LCompass mag_compass;

struct Magnetometer();

struct Magnetometer // okay, now define foo!
{
    static int * values;
};

int * Magnetometer::print_all() {
  // Read compass values
  volatile int x, y, z;

  mag_compass.read();
  x = mag_compass.getX();
  y = mag_compass.getY();
  z = mag_compass.getZ();

  static int values[3] = {x, y, z};
  delay(300);
  
  return values;

}

*/
