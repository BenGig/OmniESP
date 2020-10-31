#include "Buzzer.h"
#include <Arduino.h>

//###############################################################################
//  Buzzer
//###############################################################################

Buzzer::Buzzer(string name, LOGGING &logging, TopicQueue &topicQueue,
  int GPIOoutputPin)
  : Module (name, logging, topicQueue), pin(GPIOoutputPin)
  {}

void Buzzer::start() {
  Module::start();
  pinMode(pin, OUTPUT);
  digitalWrite(pin, HIGH);
}

//...............................................................................
// getVersion
//...............................................................................
String Buzzer::getVersion() {
  return  String(Buzzer_Name) + " v" + String(Buzzer_Version);
}

void Buzzer::setBuzzer(int mode) {
  buzz_active = mode;
}

int Buzzer::getBuzzer() {
  return buzz_active;
}

void Buzzer::handle() {
  unsigned long currentMillis = millis();
  if (buzz_active == buzz_mode_silent) {
    // nothing to do
    return;
  }

  // we're signaling. Set timing according to mode
  if (buzz_active == buzz_mode_warn) {
    currentNoiseInterval = buzz_noiseIntervalWarn;
    currentSilenceInterval = buzz_silenceIntervalWarn;
  } else if (buzz_active == buzz_mode_alert) {
    currentNoiseInterval = buzz_noiseIntervalAlert;
    currentSilenceInterval = buzz_silenceIntervalAlert;
  }

  if (buzz_throtteling) {
    currentInterval = currentSilenceInterval;
  } else {
    currentInterval = currentNoiseInterval;
  }

  // state change?
  if (currentMillis - buzz_previousMillis >= currentInterval) {
    buzz_previousMillis = currentMillis;
    if (buzz_throtteling) {
      // switch to noise
      digitalWrite(pin, LOW);
      buzz_throtteling = false;
    } else {
      digitalWrite(pin, HIGH);
      buzz_throtteling = true;
    }
  }

}
