volatile String mode;

void setup() {
  Serial.begin(9600);     // opens serial port, sets data rate to 9600 bps
}

void loop() {

  // read the sensor:

  if (Serial.available() > 0) {

    int mode = Serial.read();
    switch (mode) {
      case 'k':
        Serial.print("Entering KEYPAD mode\n");
        break;

      case 'r':
        Serial.print("Entering REMOTE mode\n");
        break;
        
      default:
        break;

    }
  }
}
