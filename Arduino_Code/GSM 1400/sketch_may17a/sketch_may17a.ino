#define LED 0  // The pin the LED is connected to
void setup() {
  pinMode(LED, OUTPUT); // Declare the LED as an output
}

void loop() {
  digitalWrite(LED, 1); // Turn the LED on
  delay(300);
  digitalWrite(LED, 0); // Turn the LED on
  delay(300);
}
