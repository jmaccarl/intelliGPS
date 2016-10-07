int led = D7;  // The on-board LED

void setup() {
  pinMode(led, OUTPUT);
          
  // Subscribe to the integration response event
  Particle.subscribe("hook-response/data", myHandler, MY_DEVICES);
}

void myHandler(const char *event, const char *data) {
  // Handle the integration response
}


void loop() {
  digitalWrite(led, HIGH);   // Turn ON the LED

  String temp = String(random(0, 100));
  Particle.publish("temp", temp, PRIVATE);
  delay(30000);               // Wait for 30 seconds

  digitalWrite(led, LOW);    // Turn OFF the LED
  delay(30000);               // Wait for 30 seconds
}
