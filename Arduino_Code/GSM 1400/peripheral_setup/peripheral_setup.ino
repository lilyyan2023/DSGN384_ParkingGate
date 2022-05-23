#include <Wire.h>
#include <config.h>
#include <ds3231.h>
 
struct ts t; //déclaration variable t 
 
void setup() {
  Serial.begin(9600);
  Wire.begin();
  DS3231_init(DS3231_INTCN);
 
  t.hour=12; // données pour mettre à l'heure l'horloge
  t.min=22;
  t.sec=0;
  t.mday=18;
  t.mon=12;
  t.year=2016;
 
  DS3231_set(t); // mise à l'heure de l'horloge
}
 
void loop() {
  // put your main code here, to run repeatedly:
  DS3231_get(&t);
  Serial.print("date : ");
  Serial.print(t.mday);
  Serial.print("/");
  Serial.print(t.mon);
  Serial.print("/");
  Serial.print(t.year);
  Serial.print("\t Heure : ");
  Serial.print(t.hour);
  Serial.print(":");
  Serial.print(t.min);
  Serial.print(".");
  Serial.println(t.sec);
 
  delay(500);
}
