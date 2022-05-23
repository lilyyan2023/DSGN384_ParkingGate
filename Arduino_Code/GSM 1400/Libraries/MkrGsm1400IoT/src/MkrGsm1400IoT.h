/*
  This file is part of the MkrGsm1400IoT library.
  It depends on the MKR GSM library.
  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.
  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.
  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
  
  23. Sep. 2020
  Author: Philip Decker
*/


#ifndef MkrGsm1400IoT_H
#define MkrGsm1400IoT_H
#include <Arduino.h>


void    resetUblox();

int     unlockSim(String pin),
        checkSimStatus(), 
        writeSim(uint8_t adress, uint16_t input_data),
        readSim(uint8_t adress);


#endif
