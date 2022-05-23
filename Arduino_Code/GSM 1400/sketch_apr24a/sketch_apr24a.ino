/*Hologram Adaptation of Arduino's GSM Web Client Example Sketch used to send messages to Hologram's Cloud


Arduino information:

  Web client

 Circuit:
 * MKR GSM 1400 board
 * Antenna
 * SIM card with a data plan

 created 8 Mar 2012
 by Tom Igoe

modified 15 May 2017
by Maiky Iberkleid
*/

// libraries
#include <MKRGSM.h>

//We replaced "arduino_secrets.h" as all the information for that is publicly available from https://hologram.io/docs/guide/connect/connect-device/#apn-settings

const char PINNUMBER[] = " ";
// APN data
const char GPRS_APN[] = "hologram";
const char GPRS_LOGIN[] = " ";
const char GPRS_PASSWORD[] = " ";


// initialize the library instance
GPRS gprs;
GSM gsmAccess;

// Hologram's Embedded API (https://hologram.io/docs/reference/cloud/embedded/) URL and port
char server[] = "cloudsocket.hologram.io";
int port = 9999;

void setup() {
  // initialize serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect. Needed for native USB port only
  }

  Serial.println("Starting Arduino web client.");
  // connection state
  boolean connected = false;

  // After starting the modem with GSM.begin()
  // attach to the GPRS network with the APN, login and password
  while (!connected) {
     Serial.println("Begin MSM Access");
    //Serial.println(gsmAccess.begin()); //Uncomment for testing
    
    if ((gsmAccess.begin() == GSM_READY) &&
        (gprs.attachGPRS(GPRS_APN, GPRS_LOGIN, GPRS_PASSWORD) == GPRS_READY)) {
      connected = true;
      Serial.println("GSM Access Success");
    } 
    else {
      Serial.println("Not connected");
      delay(1000);
    }
  }
}

void loop() {
}
