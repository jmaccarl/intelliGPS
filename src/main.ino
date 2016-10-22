
#include "TinyGPS++.h"

/*
   This sample sketch demonstrates the normal use of a TinyGPS++ (TinyGPSPlus) object.
   It requires the use of SoftwareSerial, and assumes that you have a
   4800-baud serial GPS device hooked up on pins 4(rx) and 3(tx).
*/

#define MEAS_INTERVAL 100

static const uint32_t GPSBaud = 9600;
char gpsString[128];
char powerString[128];
char activityString[128];

int count = 0;
int meas_count = 0;
float sensorValue = 0.0;
float measurements[MEAS_INTERVAL];
int power_val;

// The TinyGPS++ object
TinyGPSPlus gps;

void setup()
{
  Serial1.begin(GPSBaud);
  Serial.begin(9600);
}

void loop()
{
  // This sketch displays information every time a new sentence is correctly encoded.
  while (Serial1.available() > 0)
    if (gps.encode(Serial1.read()))
      displayInfo();
  
  if (millis() > 60000 && gps.charsProcessed() < 10)
  {
      Serial.println(F("No GPS detected: check wiring."));
      Particle.publish("gps-coordinates", "fail to connect", PRIVATE);
      while(1);
  }
  
}

void displayInfo()
{
  if (gps.location.isValid())
  {

    if (count >= 30) {
        char coord1[100];
        sprintf(coord1, "%f", (gps.location.lat(), gps.location.isValid(), 11, 6));
        char coord2[100];
        sprintf(coord2, "%f", (gps.location.lng(), gps.location.isValid(), 12, 6));        

        sprintf(gpsString, "%f,%f", coord1, coord2);
        Serial.print("GPS coordinates: ");
        Serial.println(gpsString);
        Particle.publish("gps-coordinates",gpsString, PRIVATE);
        count = 0;
    }
  }
  else
  {
    if (count >= 30){
        Serial.println("no data");
        Particle.publish("gps-coordinates", "no data", PRIVATE);
        count = 0;
    }
  }

  // shock sensor val
  float piezo_val = analogRead(A0);
  piezo_val = piezo_val / 1024.0 * 5.0;
  Serial.print("Vibration val: ");
  Serial.println(piezo_val);
  // current sensor val
  sensorValue = analogRead(A1);
  Serial.print("Power val: ");
  Serial.println(sensorValue);
  measurements[meas_count] = sensorValue;

  if (meas_count >= MEAS_INTERVAL - 1) {
    // avg current reading
    int sum = 0;
    for(int i = 0; i < MEAS_INTERVAL; i++){
      sum += measurements[i];
    }
    power_val = -1*sum/((float)MEAS_INTERVAL);
    sprintf(powerString, "%f", power_val);
    sprintf(activityString, "%f", piezo_val);
    Particle.publish("gps-power",powerString, PRIVATE);
    Particle.publish("activity-level",activityString, PRIVATE);
    meas_count = 0;
  }
      
  meas_count++;
  count++;

}
