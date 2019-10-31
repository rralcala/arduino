/*
  Play a random track when the proximity sensor detects an object
  below a threshold (like: someone passes by)
*/
#include <MD_YX5300.h>

//#define DEBUG
// Connections for serial interface to the YX5300 module
#define ARDUINO_RX 11    // connect to TX of MP3 Player module
#define ARDUINO_TX 12

// Flame leds
#define LED1 3

// Sonar pins
#define TRIGGER_PIN 9
#define ECHO_PIN 10

#define LOOP_PRINT 100
#define REQ_DETECTIONS 4
// vars
bool playerPause = true;  // true if player is currently paused
bool canplay = true;
int pct;
int detected;

// Define global variables
MD_YX5300 mp3(ARDUINO_RX, ARDUINO_TX);

float ping() {
  noInterrupts();
    digitalWrite(TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(TRIGGER_PIN, LOW);
  
    float duration = pulseIn(ECHO_PIN, HIGH);
  interrupts();
  
  return (duration*.0343)/2;
}

void mayPlay() {
  if(random(3) == 1) {
    #ifdef DEBUG
    Serial.print("DETECTED: ");
    Serial.println(distance);
    #endif
    if (canplay) {
      int track = random(10);
      // the player reports files at odd numbers only, may not be always the case.
      track += (track % 2 == 0 ? 1 : 0);
      mp3.playTrack(track);
      canplay = false;
    }
    
    analogWrite(LED1, 100); 
  }
}

void debugDistance(float distance) {
  if(pct == 0) {
    Serial.print("Distance: ");
    Serial.println(distance);
    pct = LOOP_PRINT;
  }
  pct -= 1;  
}

void setup() {
  pinMode(LED1, OUTPUT);
  pinMode(TRIGGER_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  Serial.begin(57600);
  
  mp3.begin();
  mp3.volume(6);
  mp3.setSynchronous(true);
  mp3.repeat(false);
  pct = LOOP_PRINT;
  detected = REQ_DETECTIONS;
}

void loop() {
  float distance = ping();

  // Print distance every LOOP_PRINT loops
  #ifdef DEBUG
  debugDistance(distance);
  #endif
  
  if (distance < 80.0) {
    detected -= 1;
    if (detected == 0) { 
      mayPlay();
      delay(1000);
      detected = REQ_DETECTIONS;
    }
  } else {
    analogWrite(LED1, 0);
    canplay = true; 
  }
  
  delay(1);
}
