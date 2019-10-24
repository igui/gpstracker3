#include <Arduino.h>
#include <SoftwareSerial.h>
#include "Sim808Tracker.h"
#include "config.h"

extern HardwareSerial Serial;

const int LED1 = 12;
const int LED2 = 13;

void onInit() {
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, HIGH);
}

void onReadGPS() {
  digitalWrite(LED1, HIGH);
  digitalWrite(LED2, LOW);
}

void onTransmit() {
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);
}

void onFinish() {
  digitalWrite(LED1, LOW);
  digitalWrite(LED2, LOW);
}

const Sim808::Parameters params({ 
    .apn = apn,
    .user = user,
    .password = password,
    .host = host,
    .port = 35370,
    .receivePin = 2,
    .transmitPin = 3,
    .onInit = &onInit,
    .onReadGPS = &onReadGPS,
    .onTransmit = &onTransmit,
    .onError = &onFinish,
    .onFinish = &onFinish,
});

Sim808::Tracker tracker(params);

void setup() {
    // Initial Serial port communication
    Serial.begin(9600);
    // Init LED
    pinMode(LED1, OUTPUT);
    pinMode(LED2, OUTPUT);
    digitalWrite(LED1, HIGH);
    digitalWrite(LED2, HIGH);
    // Initialize random seed generator
    randomSeed(analogRead(0));
    
    // Initialize tracker
    tracker.setup();
    Serial.println(F("SETUP DONE"));
}

void loop() {
    tracker.run();
}