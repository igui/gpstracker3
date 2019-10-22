#include <Arduino.h>
#include <SoftwareSerial.h>

#include "Sim808Tracker.h"

extern HardwareSerial Serial;

const char apn[] PROGMEM = "antel.lte";
const char user[] PROGMEM = "";
const char password[] PROGMEM = "";
const char host[] PROGMEM = "home.ignacioavas.com";

const Sim808::Parameters params({ 
    .apn = apn,
    .user = user,
    .password = password,
    .host = host,
    .port = 35370,
    .receivePin = 2,
    .transmitPin = 3,
    .onInit = NULL,
    .onReadGPS = NULL,
    .onTransmit = NULL
});

Sim808::Tracker tracker(params);

const int LED1 = 12;
const int LED2 = 13;

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