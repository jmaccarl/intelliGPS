
#include "TinyGPS++.h"

#define MEAS_INTERVAL 250

int GPS_POWER_CNTL_PIN = D6;

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
  pinMode(GPS_POWER_CNTL_PIN, OUTPUT);
}

void loop()
{
  digitalWrite(GPS_POWER_CNTL_PIN, HIGH);
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

    if (count >= 60) {
        //char coord1[100];
        //sprintf(coord1, "%f", (gps.location.lat(), gps.location.isValid(), 11, 6));
        //char coord2[100];
        //sprintf(coord2, "%f", (gps.location.lng(), gps.location.isValid(), 12, 6));        

        sprintf(gpsString, "%f,%f", gps.location.lat(), gps.location.lng());
        Serial.print("GPS coordinates: ");
        Serial.println(gpsString);
        Particle.publish("gps-coordinates",gpsString, PRIVATE);
        count = 0;
    }
  }
  else
  {
    if (count >= 60){
        Serial.println("no data");
        //Particle.publish("gps-coordinates", "no data", PRIVATE);
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
    float sum = 0;
    for(int i = 0; i < MEAS_INTERVAL; i++){
      sum += measurements[i];
    }
    power_val = sum/((float)MEAS_INTERVAL);
    sprintf(powerString, "%f", power_val);
    sprintf(activityString, "%f", piezo_val);
    Particle.publish("gps-power",powerString, PRIVATE);
    Particle.publish("activity-level",activityString, PRIVATE);
    meas_count = 0;
  }
      
  meas_count++;
  count++;

}
