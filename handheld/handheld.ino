#include <arduino-timer.h>


#define DIAL_PREFIX 'd'

Timer<1, micros> micTimer;
Timer<2, millis> dialTimer;


// Mic timer callback
bool readMic() {
  Serial.println(analogRead(A0));
  return true;
}

// Dial timer callback
bool readDial() {
  Serial.print(DIAL_PREFIX); // Prepend value with dial prefix
  Serial.println(analogRead(A1));
  return true;
}


void setup() {
  // Initialize serial communication
  Serial.begin(2000000);

  // Initialize timers
  micTimer.every(300, readMic);
  dialTimer.every(30, readDial);
}


void loop() {
  // Tick timers
  micTimer.tick();
  dialTimer.tick();
}
