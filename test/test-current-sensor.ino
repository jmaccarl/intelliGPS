// Code used to test the current sensor

const int analogInPin = A0;

int sensorValue = 0;        // value read from the sensor
int arraySize = 100;
int measurements[100];
int count = 0;

void setup() {
  // initialize serial communications at 9600 bps:
  Serial.begin(9600);
}

void loop() {
  // read the analog in value:
  sensorValue = analogRead(analogInPin);
  measurements[count] = sensorValue;

  // print the results to the serial monitor:
  if(count >= arraySize - 1) {
    int sum = 0;
    for(int i = 0; i < arraySize; i++){
      sum += measurements[i];
    }
    Serial.print("sensor = ");
    Serial.print(sum/(-1*arraySize));
    Serial.print("\n");
    count = 0;
  }

  delay(5);
  count++;
}



