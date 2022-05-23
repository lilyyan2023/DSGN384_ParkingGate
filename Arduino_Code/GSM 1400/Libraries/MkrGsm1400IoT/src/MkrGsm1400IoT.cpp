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

#include "Modem.h"
#include "MkrGsm1400IoT.h"


String response;


void resetUblox() {
  pinMode(GSM_RESETN, OUTPUT);
  digitalWrite(GSM_RESETN, HIGH);
  delay(100);
  digitalWrite(GSM_RESETN, LOW);
}

int unlockSim(String pin) {
  MODEM.sendf("AT+CPIN=\"%s\"", pin.c_str());
  if (MODEM.waitForResponse(10000) == 1) {
    return 1; //sim unlocked
  }
  int checkUp = checkSimStatus();
  if (checkUp == -1) {
    return -1; //wrong pin
  }
  if (checkUp == -2) {
    return -2; //need puk
  }
  return -3;
}



int checkSimStatus () {
  for (unsigned long start = millis(); (millis() - start) < 1000;) {
    MODEM.send("AT+CPIN?");
    if (MODEM.waitForResponse(10000, &response) == 1) {
      if (response.startsWith("+CPIN: ")) {
        if (response.endsWith("READY")) {
          return 1; //SIM ready
        } else if (response.endsWith("SIM PIN")) {
          return -1; // sim pin required
        } else if (response.endsWith("SIM PUK")) {
          return -2; // sim puk required
        } else {
          return -3; // other fail.
        }
      }
    }
  }
}


int writeSim (uint8_t adress, uint16_t input_data_uint16_t) {
  if (checkSimStatus() == 1) {
    MODEM.sendf("AT+CPBW=%d,\"%d\",,\"%d\"", adress, input_data_uint16_t, input_data_uint16_t );
    if (MODEM.waitForResponse(10000, &response) == 1) {
      if (response.lastIndexOf("OK\r\n")) {
        return 1; //successfull write data to phonebook
      } else {
        return -1;  //error to write data
      }
    }
  } else {
    return -2; //sim not ready
  }
}


int readSim (uint8_t adress) {
  char data_string[50];
  int i1, i2, i3, i4;
  if (checkSimStatus() == 1) {
    MODEM.sendf("AT+CPBR=%d", adress);
    if (adress > 250 || adress < 0) {
      if (Serial.available()) {
        Serial.println("insert a valid address 0 ... 250 ");
      }
    }
    if (MODEM.waitForResponse(10000, &response) == 1) {
      if (response.startsWith("+CPBR:")) {
        response.toCharArray(data_string, 50);
        if (4 == sscanf(data_string,
                        "%*[^0123456789]%d%*[^0123456789]%d%*[^0123456789]%d%*[^0123456789]%d",
                        &i1,
                        &i2,
                        &i3,
                        &i4))
        {
          return i2;
        }
      }
    }
  } else if (Serial.available()) {
    Serial.println("sim not ready");
  }
}
