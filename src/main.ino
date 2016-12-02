
#include "TinyGPS++.h"

//#define MEAS_INTERVAL 2048
#define MEAS_INTERVAL 512
//#define GPS_COUNT 120
#define GPS_COUNT 60
#define STARTUP_INT 600000

// Debug mode for more error checking statements
#define DEBUG

// Different modes of operation, only define one
//#define REG_MODE
#define INT_MODE

int GPS_POWER_CNTL_PIN = D6;

static const uint32_t GPSBaud = 9600;
char gpsString[128];
char powerString[128];
char activityString[128];

int count = 0;
int meas_count = 0;
float pow_measurements[MEAS_INTERVAL];
float vib_measurements[MEAS_INTERVAL];

float power_total = 0.0;
int power_count = 0;
float prev_lat = 0.0;
float prev_long = 0.0;


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

  // Wait for GPS to warm up
  //digitalWrite(GPS_POWER_CNTL_PIN, HIGH);
  
  // Get the battery charge level
  FuelGauge fuel;
  Serial.println("Current battery charge level: ");
  Serial.println( fuel.getSoC() );

}

void loop()
{

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
 
  
  if (millis() > 120000 && gps.charsProcessed() < 10)
  {
      Serial.println("No GPS detected: check wiring.");
      while(1);
  }
  

}

// NOTE: Cannot use serialEvent(), interferes with the GPS readings
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
    //Serial.println("Found valid GPS data");
    if (count >= GPS_COUNT) {
        float cur_lat = gps.location.lat();
        float cur_long = gps.location.lng(); 
        sprintf(gpsString, "%f,%f", cur_lat, cur_long);
        Serial.print("GPS coordinates: ");
        Serial.println(gpsString);
        /*
        if (prev_lat == 0.0){
          prev_lat = cur_lat;
          prev_long = cur_long;;
          Particle.publish("gps-coordinates",gpsString, PRIVATE);
        } else {
          if (TinyGPSPlus::distanceBetween(
            cur_lat, cur_long, prev_lat, prev_long) > 3){ */
            //Particle.publish("gps-coordinates", gpsString, PRIVATE);
        /*  }
          prev_lat = cur_lat;
          prev_long = cur_long;
        } */
        count = 0;
    }
  }
  else
  {
    if (count >= GPS_COUNT){
        Serial.println("no data");
#ifdef DEBUG
        Serial.print("Sentences that failed checksum=");
        Serial.println(gps.failedChecksum());
        Serial.print("Num sentences with fix: ");
        Serial.println(gps.sentencesWithFix());
#endif
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

    // Turn GPS power on/off depending on activity val
#ifdef REG_MODE
    if (millis() > STARTUP_INT && activity_val < 0.37) {
      Serial.println("Powering down GPS!");
      digitalWrite(GPS_POWER_CNTL_PIN, LOW);
    } else {
      Serial.println("Powering up GPS!");
      digitalWrite(GPS_POWER_CNTL_PIN, HIGH);
    }
#endif

    //Turn GPS power on/off according to some interval
#ifdef INT_MODE
    if ((millis() > STARTUP_INT) && (millis() % 60000 < 30000)) {
      Serial.println("Powering down GPS!");
      digitalWrite(GPS_POWER_CNTL_PIN, LOW);
    } else {
      Serial.println("Powering up GPS!");
      digitalWrite(GPS_POWER_CNTL_PIN, HIGH);
    }
#endif

    // Funtionality to only enable after the starting interval has elapsed
    if(millis() > STARTUP_INT) {
      // Don't publish power until after interval has passed
      Particle.publish("gps-power",powerString, PRIVATE);

      power_total += power_val;
      power_count++;
      // Print average total power so far
      Serial.print("Average power: ");
      Serial.println(power_total/(float)power_count);
    }

    // Print current average power val over the last interval
    Serial.print("Power val: ");
    Serial.println(powerString);

    // Print current average activity val
    //Particle.publish("activity-level",activityString, PRIVATE);
    Serial.print("Activity val: ");
    Serial.println(activityString);
    meas_count = 0;
  }

  meas_count++;

}
