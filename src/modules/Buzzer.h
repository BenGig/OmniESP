#pragma once
#include "framework/OmniESP/Module.h"
#include "framework/Utils/Logger.h"
#include "Setup.h"
#include "framework/OmniESP/Topic.h"

//###############################################################################
//  Buzzer
//###############################################################################

#define Buzzer_Name    "module::Buzzer"
#define Buzzer_Version "1.0.0"

//###############################################################################
//  Buzzer input
//###############################################################################

#define buzz_mode_silent 0
#define buzz_mode_warn 1
#define buzz_mode_alert 2

class Buzzer : public Module {
public:
  Buzzer(string name, LOGGING &logging, TopicQueue &topicQueue, int GPIOoutputPin);
  int pin;

  void start();
  void handle();
  String getVersion();

  void setBuzzer(int mode);
  int getBuzzer();

  unsigned long buzz_noiseInterval = 0;
  unsigned long buzz_silenceInterval = 0;

  unsigned long buzz_noiseIntervalWarn = 10;
  unsigned long buzz_silenceIntervalWarn = 10000;
  unsigned long buzz_noiseIntervalAlert = 20;
  unsigned long buzz_silenceIntervalAlert = 1000;

private:
  int buzz_active = buzz_mode_silent;
  unsigned long buzz_previousMillis = 0;
  unsigned long buzz_noiseIntervalCurrent = 0;
  unsigned long buzz_silenceIntervalCurrent = 0;
  bool buzz_throtteling = false;
  unsigned long currentNoiseInterval = 0;
  unsigned long currentSilenceInterval = 0;
  unsigned long currentInterval = 0;
};
