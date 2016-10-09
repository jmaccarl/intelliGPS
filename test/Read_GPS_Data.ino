#include <TinyGPS++.h>
/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const uint32_t GPSBaud = 9600;
char publishString[100];

// The TinyGPS++ object
TinyGPSPlus gps;

void setup()
{
  Serial1.begin(GPSBaud);

}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (Serial1.available() > 0)
    if (gps.encode(Serial1.read()))
      displayInfo();

  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    Serial.println(F("No GPS detected: check wiring."));
    while(true);
  }
}

void displayInfo()
{
  if (gps.location.isValid())
  {
    sprintf(publishString,"%f:%f:%f:%f",gps.location.lng(),gps.location.lat(),0.0,0.0);
    Particle.publish('intelliGPS',publishString, PRIVATE);
    Serial.println(publishString);

  }
  else
  {
    sprintf(publishString,"%f:%f:%f:%f",0.0,0.0,0.0,0.0);
    Particle.publish('intelliGPS',publishString, PRIVATE);
    Serial.println(publishString);
  }


}
