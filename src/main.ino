
#include "TinyGPS++.h"

#define MEAS_INTERVAL 512

int GPS_POWER_CNTL_PIN = D6;

static const uint32_t GPSBaud = 9600;
char gpsString[128];
char powerString[128];
char activityString[128];

int count = 0;
int meas_count = 0;
float pow_measurements[MEAS_INTERVAL];
float vib_measurements[MEAS_INTERVAL];

// serialEventRelated
std::string inputString = "";
bool stringComplete = false;

// The TinyGPS++ object
TinyGPSPlus gps;

void setup()
{
  Serial1.begin(GPSBaud);

  Serial.begin(9600);
  pinMode(GPS_POWER_CNTL_PIN, OUTPUT);
  // reserve 256 bytes for the inputString:
  inputString.reserve(256);
}

void loop()
{
  digitalWrite(GPS_POWER_CNTL_PIN, HIGH);

  if (stringComplete) {
    //Serial.println("String Complete!");
    for (unsigned i=0; i<inputString.length(); ++i)
    {
      if (gps.encode(inputString.at(i))) {
        displayInfo_gps();
      }
    }
    // clear the string:
    inputString = "";
    stringComplete = false;
  }

  displayInfo_sensors();
 /* 
  if (millis() > 60000 && gps.charsProcessed() < 10)
  {
      Serial.println(F("No GPS detected: check wiring."));
      //Particle.publish("gps-coordinates", "fail to connect", PRIVATE);
      while(1);
  }
  */

}

void serialEvent1() {
  while (Serial1.available()) {
    // get the new byte:
    char inChar = (char)Serial1.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}

void displayInfo_gps()
{
  if (gps.location.isValid())
  {

    if (count >= 60) {

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
        count = 0;
    }
  }
  count++;
}

void displayInfo_sensors()
{
  // shock sensor val
  float piezo_val = analogRead(A0);
  piezo_val = piezo_val / 1024.0 * 5.0;
  // current sensor val
  float current_val = analogRead(A1);
  pow_measurements[meas_count] = current_val;
  vib_measurements[meas_count] = piezo_val; 

  if (meas_count >= MEAS_INTERVAL - 1) {
    // avg current reading
    float power_sum = 0;
    float activity_sum = 0;
    for(int i = 0; i < MEAS_INTERVAL; i++){
      power_sum += pow_measurements[i];
      activity_sum += vib_measurements[i];
    }
    float power_val = power_sum/((float)MEAS_INTERVAL);
    float activity_val = activity_sum/((float)MEAS_INTERVAL);
    sprintf(powerString, "%f", power_val);
    sprintf(activityString, "%f", activity_val);

    //if(activity_val < 0.4){
    //  digitalWrite(GPS_POWER_CNTL_PIN, LOW);
    //}

    //Particle.publish("gps-power",powerString, PRIVATE);
    Serial.print("Power val: ");
    Serial.println(powerString);
    //Particle.publish("activity-level",activityString, PRIVATE);
    Serial.print("Activity val: ");
    Serial.println(activityString);
    meas_count = 0;
  }

  meas_count++;

}
