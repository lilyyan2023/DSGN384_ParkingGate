/*
  storeAndReadSimData

  This example sketch depneds on the "arduino-libraries/MKRGSM".
  Since the Arduino MKR GSM 1400 has no non-volatile memory,
  the only way to cache data is to store it into SIM card.
  This example sketch writes data to the phonebook, like writing data to an EEPROM
  of the sim card and reads it out and displays it on the serial monitor.
  So, for example, in your own code, control commands can be read from the phone book
  after a restart and used in the program code.

  but attention: the sim card memory has a limited read-write cycle.
  no warranty for defective sim cards.
  use these functions with care.

  define your sim pin in the arduino_secrets.h

  created 23. Sep 2020
  by Philip Decker
*/
#include "MkrGsm1400IoT.h"
#include "arduino_secrets.h"


unsigned long lastMillis = 0;
uint16_t      dataToWrite, 
              readData;
int unlockSimCard;              

void setup() {

  Serial.begin(115200);
  SerialGSM.begin(115200);      //using the SerialGSM from modem.h from the arduino-libraries/MKRGSM

  resetUblox();                                 // reset the ublox module
  delay(8000);                                  // waiting for ublox module coming up
  unlockSimCard = unlockSim(SECRET_PINNUMBER);  // unlock sim and get return
  if(unlockSimCard == 1){                       //unlock sim to access phonebook for store and read data - returns 1: sim ready; returns -1: wrong pin ; returns -2: need puk ; returns -3: other error
    Serial.println("sim unlocked");   
  }else{
    Serial.println(unlockSimCard);
      
  }
  delay(5000);                      //waiting for ublox module is unlocked
}

void loop() {
  if (lastMillis + 5000 < millis()) {
    lastMillis = millis();


    writeSim(1,dataToWrite);    // write uint16_t to the simcard phonebook - writeSim(uint16_t adress, uint16_t data) - adress: 0...250, data:0...32 767 - returns -1: write error; returns 1: success
    Serial.print("write to sim: ");
    Serial.println(dataToWrite);
    
    readData = readSim(1);      // read uint16_t from the simcard phonebook - readSim(uint16_t adress) - returns -2: sim not ready; returns -1: wrong adress; 
    Serial.print("data from sim: ");
    Serial.println(readData);

    dataToWrite++;  //to show data write and read works

    delay(2000);  //delay for the output on Serial monitor and to protect sim memory.
  }
}
