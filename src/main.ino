
#include "TinyGPS++.h"

/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/
static const uint32_t GPSBaud = 9600;
char publishString[1000];
int count = 0;
float sensorValue = 0.0;
int arraySize = 30;
float measurements[30];

// The TinyGPS++ object
TinyGPSPlus gps;

void setup()
{
  Serial1.begin(GPSBaud);
  //Serial.begin(9600);
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (Serial1.available() > 0)
    if (gps.encode(Serial1.read()))
      displayInfo();
  /*
  if (millis() > 5000 && gps.charsProcessed() < 10)
  {
    //Serial.println(F("No GPS detected: check wiring."));
    while(true) {
        sprintf(publishString,"{\"long\":%f,\"lat\":%f,\"power\":%f,\"activity\}":%f",1.0,1.0,1.0,1.0);
        Particle.publish("intelliGPS",publishString, PRIVATE);
        delay(30000);
    }
  }*/
  
}

void displayInfo()
{
  if (gps.location.isValid())
  {
    // shock sensor val
    float piezo_val=analogRead(A0);
    piezo_val = piezo_val / 1023.0 * 5.0;
    // current sensor val
    sensorValue = analogRead(A1);
    measurements[count] = sensorValue;

    if (count >= 30) {
        // avg current reading
        int sum = 0;
        for(int i = 0; i < arraySize; i++){
          sum += measurements[i];
        }
        power_val = sum/-30.0
        sprintf(publishString,"{\"long\":%f,\"lat\":%f,\"power\":%f,\"activity\":%f}",gps.location.lng(),gps.location.lat(),power_val,piezo_val);
        Particle.publish("intelliGPS",publishString, PRIVATE);
        count = 0;
    }
    //Serial.println(publishString);
    count++;
  }
  else
  {
    sprintf(publishString,"{\"long\":%f,\"lat\":%f,\"power\":%f,\"activity\":%f}",0.0,0.0,0.0,0.0);
    if (count >= 30){
        Particle.publish("intelliGPS",publishString, PRIVATE);
        count = 0;
    }
    //Serial.println(publishString);
    count++;
  }

}
