# MkrGsm1400IoT

This library can be seen as an extension to the MKRGSM library for the Arduino MKR GSM 1400.

As long as I am working with the MKR GSM 1400, I will continue to extend this library and add new functions if needed.

This library contains functions to access the sim card phonebook memory. So you can store variables or states on the memory and recall them after a restart.


Currently the library contains the following functions:
- Unlock Sim card
- Query Sim state
- Writing a positive integer in the phone book of the sim card
- Reading positive integers from the phone book of the sim card


----------

## **Contents**
1.  [Version](https://github.com/phlpjo/MkrGsm1400IoT/tree/master#version)
2.  [Arduino Compatibility](https://github.com/phlpjo/MkrGsm1400IoT/tree/master#Arduino_Compatibility)
3. [Installation](https://github.com/phlpjo/MkrGsm1400IoT/tree/master#Installation)
4. [Usage](https://github.com/phlpjo/MkrGsm1400IoT/tree/master#Usage)
5. [Example Code](https://github.com/phlpjo/MkrGsm1400IoT/tree/master#Example_Code)

## **Version**

- Version 1.0.0 -- Intial version

## **Arduino Compatibility**

-  This library is designed exclusively for use with the Arduino MKR GSM 1400



## **Installation**

The MkrGsm1400IoT library may be soon installed using the Arduino IDE's Library Manager. To access the Library Manager, in the Arduino IDE's menu select _Sketch->Include Library->Manage Libraries..._. In the Library Manager's search bar type MkrGsm1400IoT then select the "Install" button in the MkrGsm1400IoT entry.

At the moment this library has to be downloaded manually via github and added to arduino.
Just download the library, unzip it and add it to the folder _.../Arduino/libraries_ . The _.../Arduino/..._ folder is the one where your Arduino IDE sketches are usually located.

## **Usage**

So you know how to use the functions of this library, take a look at the examples.

## **Example Code**
**storeAndReadSimData.ino**
- Write and read integers from 0 to 65535 to/from the Sim-Card
- Store up to 250 different variable (testet with an O2 nano sim card (german))


